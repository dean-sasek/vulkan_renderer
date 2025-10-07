#include "renderer.h"
#include "../application/application.h"

void Renderer::setApplication(Application& application) {
	this->application = &application;
}

const VkPhysicalDevice Renderer::getPhysicalDevice() {
	return physicalDevice;
}


VkDevice Renderer::getDevice() {
	return device;
}

void Renderer::createSurface() {
	log_info("Creating window surface...");

	VkResult result = glfwCreateWindowSurface(instance, application->window.glfwWindow, nullptr, &surface);

	if (result != VK_SUCCESS) {
		log_error("Failed to create window surface!");
	}
	else if (result == VK_SUCCESS) {
		log_info("Successfully created window surface!");
	}
}

void Renderer::setPhysicalDevice(VkPhysicalDevice physicalDevice) {
	this->physicalDevice = physicalDevice;
}

void Renderer::createLogicalDevice() {
	Renderer::queueFamilyIndices indices = findQueueFamilies(getPhysicalDevice());

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		createInfo.queueFamilyIndex = queueFamily;
		createInfo.queueCount = 1;
		createInfo.pQueuePriorities = &queuePriority;
		
		queueCreateInfos.push_back(createInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

	if (result != VK_SUCCESS) {
		log_error("Failed to create logical device!");
	}
	else if (result == VK_SUCCESS) {
		log_info("Successfully created logical device!");
	}
}

const VkRenderPass Renderer::getRenderPass() {
	return renderPass;
}

void Renderer::init(Application& application) {
	log_info("Initializing renderer...");

	this->application = &application;
	this->application->swapchain.init(application);
	this->application->pipelines.init(application);
	this->application->shaders.init(application);

	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	this->application->swapchain.createSwapchain();
	this->application->swapchain.createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	this->application->swapchain.createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();

	log_info("Renderer initialized!");
}

void Renderer::createGraphicsPipeline() {
	log_info("Creating graphics pipeline...");

	Pipelines::pipelineStructure pipelineStructure{};

	pipelineStructure.vertexShaderPath = "src/renderer/shaders/vert.spv";
	pipelineStructure.fragmentShaderPath = "src/renderer/shaders/frag.spv";
	
	pipelineStructure.vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineStructure.vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
	pipelineStructure.vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
	pipelineStructure.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
	pipelineStructure.vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

	pipelineStructure.inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineStructure.inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineStructure.inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	pipelineStructure.rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineStructure.rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	pipelineStructure.rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineStructure.rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineStructure.rasterizationStateCreateInfo.lineWidth = 1.0f;
	pipelineStructure.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	pipelineStructure.rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipelineStructure.rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	pipelineStructure.rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	pipelineStructure.rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	pipelineStructure.rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	pipelineStructure.multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineStructure.multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	pipelineStructure.multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pipelineStructure.multisampleStateCreateInfo.minSampleShading = 1.0f;
	pipelineStructure.multisampleStateCreateInfo.pSampleMask = nullptr;
	pipelineStructure.multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	pipelineStructure.multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	pipelineStructure.colorBlendAttachmentStateCreateInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	pipelineStructure.colorBlendAttachmentStateCreateInfo.blendEnable = VK_FALSE;
	pipelineStructure.colorBlendAttachmentStateCreateInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	pipelineStructure.colorBlendAttachmentStateCreateInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineStructure.colorBlendAttachmentStateCreateInfo.colorBlendOp = VK_BLEND_OP_ADD;
	pipelineStructure.colorBlendAttachmentStateCreateInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	pipelineStructure.colorBlendAttachmentStateCreateInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineStructure.colorBlendAttachmentStateCreateInfo.alphaBlendOp = VK_BLEND_OP_ADD;

	pipelineStructure.colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineStructure.colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	pipelineStructure.colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	pipelineStructure.colorBlendStateCreateInfo.attachmentCount = 1;
	pipelineStructure.colorBlendStateCreateInfo.pAttachments = &pipelineStructure.colorBlendAttachmentStateCreateInfo;
	pipelineStructure.colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	pipelineStructure.colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	pipelineStructure.colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	pipelineStructure.colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	static std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
	pipelineStructure.dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineStructure.dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	pipelineStructure.dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

	VkPipelineLayout pipelineLayout = this->application->pipelines.createPipelineLayout();
	graphicsPipelineLayout = pipelineLayout;

	pipelineStructure.depthStencilStateCreateInfo = nullptr;

	pipelineStructure.pipelineLayout = pipelineLayout;
	pipelineStructure.renderPass = renderPass;
	pipelineStructure.subpass = 0;

	VkPipeline pipeline = this->application->pipelines.createPipeline(pipelineStructure);
	graphicsPipeline = pipeline;

	log_info("Successfully created graphics pipeline!");
}

void Renderer::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = application->swapchain.getImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription{};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;

	VkSubpassDependency subpassDependency{};
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDescription;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &subpassDependency;

	VkResult result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);

	if (result != VK_SUCCESS) {
		log_error("Failed to create render pass!");
	}
	else {
		log_info("Successfully created render pass!");
	}
}

void Renderer::createCommandPool() {
	Renderer::queueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VkResult commandPoolResult = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);

	if (commandPoolResult != VK_SUCCESS) {
		log_error("Failed to create command pool!");
	}
	else {
		log_info("Successfully created command pool!");
	}
}

void Renderer::createCommandBuffers() {
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	VkResult commandBufferResult = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data());

	if (commandBufferResult != VK_SUCCESS) {
		log_error("Failed to allocate command buffers!");
	}
	else {
		log_info("Successfully allocated command buffers!");
	}
}

void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	VkResult commandBufferBeginInfoResult = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	if (commandBufferBeginInfoResult != VK_SUCCESS) {
		log_error("Failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = application->swapchain.getFramebuffers()[imageIndex];
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = application->swapchain.getExtent();

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(application->swapchain.getExtent().width);
	viewport.height = static_cast<float>(application->swapchain.getExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = application->swapchain.getExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	VkResult commandBufferResult = vkEndCommandBuffer(commandBuffer);

	if (commandBufferResult != VK_SUCCESS) {
		log_error("Failed to record command buffer!");
	};
}

void Renderer::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult imageAvailableSemaphoreResult = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]);
		VkResult renderFinishedSemaphoreResult = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]);
		VkResult inFlightFenceResult = vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]);

		if (imageAvailableSemaphoreResult != VK_SUCCESS || renderFinishedSemaphoreResult != VK_SUCCESS || inFlightFenceResult != VK_SUCCESS) {
			log_error("Failed to create semaphores and fences!");
		}
		else {
			log_info("Successfully created semaphores & fences!");
		}
	}
}

void Renderer::cleanup() {
	log_info("Cleaning up renderer...");

	vkDeviceWaitIdle(device);

	application->swapchain.cleanup();

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);

	vkDestroyRenderPass(device, renderPass, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers) {
		destroyDebugUtilsMessengerExt(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	log_info("Renderer cleaned up!");
}

void Renderer::drawFrame() {
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult acquireNextImageResult = vkAcquireNextImageKHR(device, application->swapchain.getSwapchain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR || acquireNextImageResult == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		application->swapchain.recreateSwapchain();

		log_info("Swap chain out of date, recreating...");

		return;
	}
	else if (acquireNextImageResult != VK_SUCCESS) {
		log_error("Failed to acquire swap chain image!");
	}

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]};
	
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult queueSubmitResult = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);

	if (queueSubmitResult != VK_SUCCESS) {
		log_error("Failed to submit draw!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { application->swapchain.getSwapchain()};
	
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(graphicsQueue, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::createInstance() {
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		log_error("Validation layers requested, but not supported!");
	}
	else {
		log_info("Validation layers enabled!");
	}

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	applicationInfo.pApplicationName = application->window.getWindowName(*application);
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

	applicationInfo.pEngineName = "None";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	applicationInfo.apiVersion = VK_API_VERSION_1_4;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		log_error("Failed to create Vulkan instance!");
	}
	else {
		log_info("Successfully created Vulkan instance!");
	}
}

bool Renderer::checkValidationLayerSupport() {
	uint32_t layerCount;
	
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;

				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> Renderer::getRequiredExtensions() {
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	//log_info(std::string("Validation layer: ") + pCallbackData->pMessage);

	return VK_FALSE;
}

void Renderer::setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};

	populateDebugMessengerCreateInfo(createInfo);

	if (createDebugUtilsMessengerExt(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		log_error("Failed to create debug messenger!");
	}
	else {
		log_info("Successfully created debug messenger!");
	}
}

VkResult Renderer::createDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)

	vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Renderer::destroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)

	vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void Renderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

void Renderer::pickPhysicalDevice() {
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		log_error("Failed to find GPU with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isPhysicalDeviceSuitable(device)) {
			setPhysicalDevice(device);

			log_info("Successfully picked physical device!");

			break;
		}
	}

	if (getPhysicalDevice() == VK_NULL_HANDLE) {
		log_error("Failed to find a suitable physical device!");
	}
}

bool Renderer::isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice) {
	indices = findQueueFamilies(physicalDevice);

	bool extensionsSupported = checkPhysicalDeviceExtensionSupport(physicalDevice);

	bool swapChainAdequate = false;
	
	if (extensionsSupported) {
		Swapchain::swapchainSupportDetails swapchainSupport = application->swapchain.querySwapchainSupport(*application, physicalDevice);
		swapChainAdequate = !swapchainSupport.surfaceFormats.empty() && !swapchainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Renderer::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
	uint32_t extensionCount;

	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
	
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

const Renderer::queueFamilyIndices Renderer::findQueueFamilies(VkPhysicalDevice physicalDevice) {
	Renderer::queueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;

			//log_info("Graphics support found!");

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;

				log_info("Present support found!");
			}
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}