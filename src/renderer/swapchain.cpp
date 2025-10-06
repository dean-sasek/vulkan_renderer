#include "../application/application.h"

VkFormat Swapchain::getImageFormat(Application& application) {
	return application.swapchain.imageFormat;
}

VkExtent2D Swapchain::getExtent(Application& application) {
	return application.swapchain.imageExtent;
}

std::vector<VkFramebuffer> Swapchain::getFramebuffers(Application& application) {
	return application.swapchain.framebuffers;
}

VkSwapchainKHR Swapchain::getSwapchain(Application& application) {
	return application.swapchain.swapchain;
}

void Swapchain::cleanup(Application& application) {
	log_info("Cleaning up swapchain...");

	for (auto framebuffer : application.swapchain.framebuffers) {
		vkDestroyFramebuffer(application.renderer.getDevice(application), framebuffer, nullptr);
	}

	for (auto imageView : application.swapchain.imageViews) {
		vkDestroyImageView(application.renderer.getDevice(application), imageView, nullptr);
	}

	vkDestroySwapchainKHR(application.renderer.getDevice(application), application.swapchain.swapchain, nullptr);

	log_info("Cleaned up swapchain!");
}

void Swapchain::createSwapchain(Application& application) {
	Swapchain& swapchain = application.swapchain;

	Swapchain::swapchainSupportDetails swapchainSupportDetails = swapchain.querySwapchainSupport(application, application.renderer.getPhysicalDevice(application));

	VkSurfaceFormatKHR surfaceFormat = swapchain.chooseSwapchainSurfaceFormat(swapchainSupportDetails.surfaceFormats);
	VkPresentModeKHR presentMode = swapchain.chooseSwapchainPresentMode(swapchainSupportDetails.presentModes);
	VkExtent2D swapChainExtent = swapchain.chooseSwapchainExtent(application, swapchainSupportDetails.surfaceCapabilities);

	uint32_t imageCount = swapchainSupportDetails.surfaceCapabilities.minImageCount + 1;

	if (swapchainSupportDetails.surfaceCapabilities.maxImageCount > 0 && imageCount >> swapchainSupportDetails.surfaceCapabilities.maxImageCount) {
		imageCount = swapchainSupportDetails.surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = application.renderer.surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapChainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	Renderer::queueFamilyIndices indices = application.renderer.findQueueFamilies(application, application.renderer.getPhysicalDevice(application));
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapchainSupportDetails.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(application.renderer.getDevice(application), &createInfo, nullptr, &swapchain.swapchain);

	if (result != VK_SUCCESS) {
		log_error("Failed to create swapchain!");
	}
	else {
		log_info("Successfully created swapchain!");
	}

	vkGetSwapchainImagesKHR(application.renderer.getDevice(application), swapchain.swapchain, &imageCount, nullptr);
	swapchain.images.resize(imageCount);
	vkGetSwapchainImagesKHR(application.renderer.getDevice(application), swapchain.swapchain, &imageCount, swapchain.images.data());

	swapchain.imageFormat = surfaceFormat.format;
	swapchain.imageExtent = swapChainExtent;

	application.swapchain = swapchain;
}

void Swapchain::recreateSwapchain(Application& application) {
	int width = 0, height = 0;
	glfwGetFramebufferSize(application.window.glfwWindow, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(application.window.glfwWindow, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(application.renderer.getDevice(application));

	application.swapchain.cleanup(application);

	application.swapchain.createSwapchain(application);
	application.swapchain.createImageViews(application);
	application.swapchain.createFramebuffers(application);
}

void Swapchain::createImageViews(Application& application) {
	application.swapchain.imageViews.resize(application.swapchain.images.size());

	for (size_t i = 0; i < application.swapchain.images.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = application.swapchain.images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = application.swapchain.imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(application.renderer.getDevice(application), &createInfo, nullptr, &application.swapchain.imageViews[i]) != VK_SUCCESS) {
			log_error("failed to create image views!");
		}
	}
}

Swapchain::swapchainSupportDetails Swapchain::querySwapchainSupport(Application& application, VkPhysicalDevice physicalDevice) {
	Swapchain::swapchainSupportDetails swapchainSupportDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, application.renderer.surface, &swapchainSupportDetails.surfaceCapabilities);

	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, application.renderer.surface, &surfaceFormatCount, nullptr);

	if (surfaceFormatCount != 0) {
		swapchainSupportDetails.surfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, application.renderer.surface, &surfaceFormatCount, swapchainSupportDetails.surfaceFormats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, application.renderer.surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		swapchainSupportDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, application.renderer.surface, &presentModeCount, swapchainSupportDetails.presentModes.data());
	}

	return swapchainSupportDetails;
}

void Swapchain::createFramebuffers(Application& application) {
	application.swapchain.framebuffers.resize(application.swapchain.imageViews.size());

	for (size_t i = 0; i < application.swapchain.imageViews.size(); i++) {
		VkImageView attachments[] = {
			application.swapchain.imageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = application.renderer.getRenderPass(application);
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = application.swapchain.imageExtent.width;
		framebufferInfo.height = application.swapchain.imageExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(application.renderer.getDevice(application), &framebufferInfo, nullptr, &application.swapchain.framebuffers[i]) != VK_SUCCESS) {
			log_error("Failed to create framebuffer!");
		}
	}
}

VkSurfaceFormatKHR Swapchain::chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapchainExtent(Application& application, const VkSurfaceCapabilitiesKHR& surfaceCapabilities) {
	#ifdef max
	#undef max
	#endif

	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return surfaceCapabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(application.window.glfwWindow, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

		return actualExtent;
	}
}