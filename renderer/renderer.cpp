#include <iostream>
#include <cstring>

#include "renderer.h"
#include "../window/window.h"
#include "../fileSystem/fileSystem.h"
#include "swapchain.h"

Renderer _renderer;

VkPhysicalDevice Renderer::getPhysicalDevice() {
	return _renderer.physicalDevice;
}


VkDevice Renderer::getDevice() {
	return _renderer.device;
}

void Renderer::createSurface() {
	std::cout << "Creating window surface..." << std::endl;

	VkResult result = glfwCreateWindowSurface(_renderer.instance, _window.glfwWindow, nullptr, &_renderer.surface);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
	else if (result == VK_SUCCESS) {
		std::cout << "Successfully created window surface!" << std::endl;
	}
}

void Renderer::setPhysicalDevice(VkPhysicalDevice physicalDevice) {
	_renderer.physicalDevice = physicalDevice;
}

void Renderer::createLogicalDevice() {
	Renderer::queueFamilyIndices indices = _renderer.findQueueFamilies(_renderer.getPhysicalDevice());

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
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(_renderer.deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = _renderer.deviceExtensions.data();

	VkResult result = vkCreateDevice(_renderer.physicalDevice, &deviceCreateInfo, nullptr, &_renderer.device);

	vkGetDeviceQueue(_renderer.device, _renderer.indices.graphicsFamily.value(), 0, &_renderer.graphicsQueue);
	vkGetDeviceQueue(_renderer.device, _renderer.indices.presentFamily.value(), 0, &_renderer.presentQueue);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device!");
	}
	else if (result == VK_SUCCESS) {
		std::cout << "Successfully created logical device!" << std::endl;
	}
}

VkRenderPass Renderer::getRenderPass() {
	return _renderer.renderPass;
}

void Renderer::init() {
	std::cout << "Initializing renderer..." << std::endl;

	_renderer.createInstance();
	_renderer.setupDebugMessenger();
	_renderer.createSurface();
	_renderer.pickPhysicalDevice();
	_renderer.createLogicalDevice();
	_swapchain.createSwapchain();
	_swapchain.createImageViews();
	_renderer.createRenderPass();
	_renderer.createGraphicsPipeline();
	_swapchain.createFramebuffers();
	_renderer.createCommandPool();
	_renderer.createCommandBuffers();
	_renderer.createSyncObjects();

	std::cout << "Renderer initialized!" << std::endl;
}

void Renderer::createGraphicsPipeline() {
	auto vertexShaderCode = fileSystem.readFile("renderer/shaders/vert.spv");
	auto fragmentShaderCode = fileSystem.readFile("renderer/shaders/frag.spv");

	VkShaderModule vertexShaderModule = _renderer.createShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = _renderer.createShaderModule(fragmentShaderCode);

	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
	vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageCreateInfo.module = vertexShaderModule;
	vertexShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo{};
	fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageCreateInfo.module = fragmentShaderModule;
	fragmentShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateCreateInfo.lineWidth = 1.0f;
	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleStateCreateInfo.minSampleShading = 1.0f;
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachmentState.blendEnable = VK_FALSE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendStateCreateInfo.attachmentCount = 1;
	colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
	colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	VkResult pipelineLayoutResult = vkCreatePipelineLayout(_renderer.device, &pipelineLayoutCreateInfo, nullptr, &_renderer.pipelineLayout);

	if (pipelineLayoutResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout!");
	}
	else {
		std::cout << "Successfully created pipeline layout!" << std::endl;
	}

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.stageCount = 2;
	graphicsPipelineCreateInfo.pStages = shaderStages;
	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
	graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	graphicsPipelineCreateInfo.layout = _renderer.pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = _renderer.renderPass;
	graphicsPipelineCreateInfo.subpass = 0;

	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;

	VkResult graphicsPipelineResult = vkCreateGraphicsPipelines(_renderer.device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &_renderer.graphicsPipeline);

	if (graphicsPipelineResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipeline!");
	}
	else {
		std::cout << "Successfully created graphics pipeline!" << std::endl;
	}

	vkDestroyShaderModule(_renderer.device, vertexShaderModule, nullptr);
	vkDestroyShaderModule(_renderer.device, fragmentShaderModule, nullptr);
}

VkShaderModule Renderer::createShaderModule(const std::vector<char>& shaderCode) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	VkShaderModule shaderModule;

	VkResult result = vkCreateShaderModule(_renderer.device, &createInfo, nullptr, &shaderModule);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module!");
	}
	else {
		std::cout << "Successfully created shader module!" << std::endl;

		return shaderModule;
	}
};

void Renderer::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = _swapchain.getImageFormat();
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

	VkResult result = vkCreateRenderPass(_renderer.device, &renderPassCreateInfo, nullptr, &_renderer.renderPass);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create render pass!");
	}
	else {
		std::cout << "Successfully created render pass!" << std::endl;
	}
}

void Renderer::createCommandPool() {
	Renderer::queueFamilyIndices queueFamilyIndices = _renderer.findQueueFamilies(_renderer.physicalDevice);

	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VkResult commandPoolResult = vkCreateCommandPool(_renderer.device, &commandPoolCreateInfo, nullptr, &_renderer.commandPool);

	if (commandPoolResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}
	else {
		std::cout << "Successfully created command pool!" << std::endl;
	}
}

void Renderer::createCommandBuffers() {
	_renderer.commandBuffers.resize(_renderer.MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = _renderer.commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = (uint32_t)_renderer.commandBuffers.size();

	VkResult commandBufferResult = vkAllocateCommandBuffers(_renderer.device, &commandBufferAllocateInfo, _renderer.commandBuffers.data());

	if (commandBufferResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers!");
	}
	else {
		std::cout << "Successfully allocated command buffers!" << std::endl;
	}
}

void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	VkResult commandBufferBeginInfoResult = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	if (commandBufferBeginInfoResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = _renderer.renderPass;
	renderPassBeginInfo.framebuffer = _swapchain.getFramebuffers()[imageIndex];
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = _swapchain.getExtent();

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _renderer.graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<uint32_t>(_swapchain.getExtent().width);
	viewport.height = static_cast<uint32_t>(_swapchain.getExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = _swapchain.getExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	VkResult commandBufferResult = vkEndCommandBuffer(commandBuffer);

	if (commandBufferResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	};
}

void Renderer::createSyncObjects() {
	_renderer.imageAvailableSemaphores.resize(_renderer.MAX_FRAMES_IN_FLIGHT);
	_renderer.renderFinishedSemaphores.resize(_renderer.MAX_FRAMES_IN_FLIGHT);
	_renderer.inFlightFences.resize(_renderer.MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	for (size_t i = 0; i < _renderer.MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult imageAvailableSemaphoreResult = vkCreateSemaphore(_renderer.device, &semaphoreCreateInfo, nullptr, &_renderer.imageAvailableSemaphores[i]);
		VkResult renderFinishedSemaphoreResult = vkCreateSemaphore(_renderer.device, &semaphoreCreateInfo, nullptr, &_renderer.renderFinishedSemaphores[i]);
		VkResult inFlightFenceResult = vkCreateFence(_renderer.device, &fenceCreateInfo, nullptr, &inFlightFences[i]);

		if (imageAvailableSemaphoreResult != VK_SUCCESS || renderFinishedSemaphoreResult != VK_SUCCESS || inFlightFenceResult != VK_SUCCESS) {
			throw std::runtime_error("Failed to create semaphores and fences!");
		}
		else {
			std::cout << "Successfully created semaphores & fences!" << std::endl;
		}
	}
}

void Renderer::cleanup() {
	std::cout << "Cleaning up renderer..." << std::endl;

	vkDeviceWaitIdle(_renderer.device);

	_swapchain.cleanup();

	vkDestroyPipeline(_renderer.device, _renderer.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(_renderer.device, _renderer.pipelineLayout, nullptr);

	vkDestroyRenderPass(_renderer.device, _renderer.renderPass, nullptr);

	for (size_t i = 0; i < _renderer.MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(_renderer.device, _renderer.renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(_renderer.device, _renderer.imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(_renderer.device, _renderer.inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(_renderer.device, _renderer.commandPool, nullptr);

	vkDestroyDevice(_renderer.device, nullptr);

	if (_renderer.enableValidationLayers) {
		_renderer.destroyDebugUtilsMessengerExt(_renderer.instance, _renderer.debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(_renderer.instance, _renderer.surface, nullptr);
	vkDestroyInstance(_renderer.instance, nullptr);

	std::cout << "Renderer cleaned up!" << std::endl;
}

void Renderer::drawFrame() {
	vkWaitForFences(_renderer.device, 1, &_renderer.inFlightFences[_renderer.currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult acquireNextImageResult = vkAcquireNextImageKHR(_renderer.device, _swapchain.getSwapchain(), UINT64_MAX, _renderer.imageAvailableSemaphores[_renderer.currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR || acquireNextImageResult == VK_SUBOPTIMAL_KHR || _renderer.framebufferResized) {
		_renderer.framebufferResized = false;
		_swapchain.recreateSwapchain();

		std::cout << "Swap chain out of date, recreating..." << std::endl;

		return;
	}
	else if (acquireNextImageResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	vkResetFences(_renderer.device, 1, &_renderer.inFlightFences[_renderer.currentFrame]);

	vkResetCommandBuffer(_renderer.commandBuffers[currentFrame], 0);
	_renderer.recordCommandBuffer(_renderer.commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { _renderer.imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_renderer.commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { _renderer.renderFinishedSemaphores[currentFrame]};
	
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult queueSubmitResult = vkQueueSubmit(_renderer.graphicsQueue, 1, &submitInfo, _renderer.inFlightFences[currentFrame]);

	if (queueSubmitResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { _swapchain.getSwapchain()};
	
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(_renderer.graphicsQueue, &presentInfo);

	_renderer.currentFrame = (_renderer.currentFrame + 1) % _renderer.MAX_FRAMES_IN_FLIGHT;
}

void Renderer::createInstance() {
	if (_renderer.enableValidationLayers && !_renderer.checkValidationLayerSupport()) {
		throw std::runtime_error("Validation layers requested, but not supported!");
	}
	else {
		std::cout << "Validation layers enabled!" << std::endl;
	}

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	applicationInfo.pApplicationName = _window.getWindowName();
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

	applicationInfo.pEngineName = "None";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	applicationInfo.apiVersion = VK_API_VERSION_1_4;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	auto extensions = _renderer.getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (_renderer.enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = _renderer.validationLayers.data();

		_renderer.populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &_renderer.instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan instance!");
	}
	else {
		std::cout << "Successfully created Vulkan instance!" << std::endl;
	}
}

bool Renderer::checkValidationLayerSupport() {
	uint32_t layerCount;
	
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : _renderer.validationLayers) {
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
	_renderer.glfwExtensions = glfwGetRequiredInstanceExtensions(&_renderer.glfwExtensionCount);

	std::vector<const char*> extensions(_renderer.glfwExtensions, _renderer.glfwExtensions + _renderer.glfwExtensionCount);

	if (_renderer.enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void Renderer::setupDebugMessenger() {
	if (!_renderer.enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};

	_renderer.populateDebugMessengerCreateInfo(createInfo);

	if (_renderer.createDebugUtilsMessengerExt(_renderer.instance, &createInfo, nullptr, &_renderer.debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create debug messenger!");
	}
	else {
		std::cout << "Successfully created debug messenger!" << std::endl;
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
	createInfo.pfnUserCallback = _renderer.debugCallback;
}

void Renderer::pickPhysicalDevice() {
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(_renderer.instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPU with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(_renderer.instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (_renderer.isPhysicalDeviceSuitable(device)) {
			_renderer.setPhysicalDevice(device);

			std::cout << "Successfully picked physical device!" << std::endl;

			break;
		}
	}

	if (_renderer.getPhysicalDevice() == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable physical device!");
	}
}

bool Renderer::isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice) {
	_renderer.indices = _renderer.findQueueFamilies(physicalDevice);

	bool extensionsSupported = _renderer.checkPhysicalDeviceExtensionSupport(physicalDevice);

	bool swapChainAdequate = false;
	
	if (extensionsSupported) {
		Swapchain::swapchainSupportDetails swapchainSupport = _swapchain.querySwapchainSupport(physicalDevice);
		swapChainAdequate = !swapchainSupport.surfaceFormats.empty() && !swapchainSupport.presentModes.empty();
	}

	return _renderer.indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Renderer::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
	uint32_t extensionCount;

	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
	
	std::set<std::string> requiredExtensions(_renderer.deviceExtensions.begin(), _renderer.deviceExtensions.end());

	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

Renderer::queueFamilyIndices Renderer::findQueueFamilies(VkPhysicalDevice physicalDevice) {
	Renderer::queueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;

			std::cout << "Graphics support found!" << std::endl;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, _renderer.surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;

				std::cout << "Present support found!" << std::endl;
			}
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}