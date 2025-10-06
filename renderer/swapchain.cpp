#include <iostream>

#include "swapchain.h"
#include "renderer.h"
#include "../window/window.h"

Swapchain _swapchain;

VkFormat Swapchain::getImageFormat() {
	return _swapchain.imageFormat;
}

VkExtent2D Swapchain::getExtent() {
	return _swapchain.imageExtent;
}

std::vector<VkFramebuffer> Swapchain::getFramebuffers() {
	return _swapchain.framebuffers;
}

VkSwapchainKHR Swapchain::getSwapchain() {
	return _swapchain.swapchain;
}

void Swapchain::cleanup() {
	std::cout << "Cleaning up swapchain..." << std::endl;

	for (auto framebuffer : _swapchain.framebuffers) {
		vkDestroyFramebuffer(_renderer.getDevice(), framebuffer, nullptr);
	}

	for (auto imageView : _swapchain.imageViews) {
		vkDestroyImageView(_renderer.getDevice(), imageView, nullptr);
	}

	vkDestroySwapchainKHR(_renderer.getDevice(), _swapchain.swapchain, nullptr);

	std::cout << "Cleaned up swapchain!" << std::endl;
}

void Swapchain::createSwapchain() {
	Swapchain& swapchain = _swapchain;

	Swapchain::swapchainSupportDetails swapchainSupportDetails = swapchain.querySwapchainSupport(_renderer.getPhysicalDevice());

	VkSurfaceFormatKHR surfaceFormat = swapchain.chooseSwapchainSurfaceFormat(swapchainSupportDetails.surfaceFormats);
	VkPresentModeKHR presentMode = swapchain.chooseSwapchainPresentMode(swapchainSupportDetails.presentModes);
	VkExtent2D swapChainExtent = swapchain.chooseSwapchainExtent(swapchainSupportDetails.surfaceCapabilities);

	uint32_t imageCount = swapchainSupportDetails.surfaceCapabilities.minImageCount + 1;

	if (swapchainSupportDetails.surfaceCapabilities.maxImageCount > 0 && imageCount >> swapchainSupportDetails.surfaceCapabilities.maxImageCount) {
		imageCount = swapchainSupportDetails.surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _renderer.surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapChainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	Renderer::queueFamilyIndices indices = _renderer.findQueueFamilies(_renderer.getPhysicalDevice());
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

	VkResult result = vkCreateSwapchainKHR(_renderer.getDevice(), &createInfo, nullptr, &swapchain.swapchain);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swapchain!");
	}
	else {
		std::cout << "Successfully created swapchain!" << std::endl;
	}

	vkGetSwapchainImagesKHR(_renderer.getDevice(), swapchain.swapchain, &imageCount, nullptr);
	swapchain.images.resize(imageCount);
	vkGetSwapchainImagesKHR(_renderer.getDevice(), swapchain.swapchain, &imageCount, swapchain.images.data());

	swapchain.imageFormat = surfaceFormat.format;
	swapchain.imageExtent = swapChainExtent;

	_swapchain = swapchain;
}

void Swapchain::recreateSwapchain() {
	int width = 0, height = 0;
	glfwGetFramebufferSize(_window.glfwWindow, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(_window.glfwWindow, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(_renderer.getDevice());

	_swapchain.cleanup();

	_swapchain.createSwapchain();
	_swapchain.createImageViews();
	_swapchain.createFramebuffers();
}

void Swapchain::createImageViews() {
	_swapchain.imageViews.resize(_swapchain.images.size());

	for (size_t i = 0; i < _swapchain.images.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _swapchain.images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _swapchain.imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_renderer.getDevice(), &createInfo, nullptr, &_swapchain.imageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

Swapchain::swapchainSupportDetails Swapchain::querySwapchainSupport(VkPhysicalDevice physicalDevice) {
	Swapchain::swapchainSupportDetails swapchainSupportDetails;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _renderer.surface, &swapchainSupportDetails.surfaceCapabilities);

	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _renderer.surface, &surfaceFormatCount, nullptr);

	if (surfaceFormatCount != 0) {
		swapchainSupportDetails.surfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _renderer.surface, &surfaceFormatCount, swapchainSupportDetails.surfaceFormats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _renderer.surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		swapchainSupportDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _renderer.surface, &presentModeCount, swapchainSupportDetails.presentModes.data());
	}

	return swapchainSupportDetails;
}

void Swapchain::createFramebuffers() {
	_swapchain.framebuffers.resize(_swapchain.imageViews.size());

	for (size_t i = 0; i < _swapchain.imageViews.size(); i++) {
		VkImageView attachments[] = {
			_swapchain.imageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _renderer.getRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = _swapchain.imageExtent.width;
		framebufferInfo.height = _swapchain.imageExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(_renderer.getDevice(), &framebufferInfo, nullptr, &_swapchain.framebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer!");
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

VkExtent2D Swapchain::chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) {
	#ifdef max
	#undef max
	#endif

	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return surfaceCapabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(_window.glfwWindow, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

		return actualExtent;
	}
}