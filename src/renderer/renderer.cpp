#include "renderer.h"
#include "../../main.h"

VkPhysicalDevice Renderer::getPhysicalDevice() {
	return application.renderer.physicalDevice;
}


VkDevice Renderer::getDevice() {
	return application.renderer.device;
}

void Renderer::createSurface() {
	log_info("Creating window surface...");

	VkResult result = glfwCreateWindowSurface(application.renderer.instance, application.window.glfwWindow, nullptr, &application.renderer.surface);

	if (result != VK_SUCCESS) {
		log_error("Failed to create window surface!");
	}
	else if (result == VK_SUCCESS) {
		log_info("Successfully created window surface!");
	}
}

void Renderer::setPhysicalDevice(VkPhysicalDevice physicalDevice) {
	application.renderer.physicalDevice = physicalDevice;
}

void Renderer::createLogicalDevice() {
	Renderer::queueFamilyIndices indices = application.renderer.findQueueFamilies(application.renderer.getPhysicalDevice());

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
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(application.renderer.deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = application.renderer.deviceExtensions.data();

	VkResult result = vkCreateDevice(application.renderer.physicalDevice, &deviceCreateInfo, nullptr, &application.renderer.device);

	vkGetDeviceQueue(application.renderer.device, application.renderer.indices.graphicsFamily.value(), 0, &application.renderer.graphicsQueue);
	vkGetDeviceQueue(application.renderer.device, application.renderer.indices.presentFamily.value(), 0, &application.renderer.presentQueue);

	if (result != VK_SUCCESS) {
		log_error("Failed to create logical device!");
	}
	else if (result == VK_SUCCESS) {
		log_info("Successfully created logical device!");
	}
}

VkRenderPass Renderer::getRenderPass() {
	return application.renderer.renderPass;
}

void Renderer::init() {
	log_info("Initializing renderer...");

	application.renderer.createInstance();
	application.renderer.setupDebugMessenger();
	application.renderer.createSurface();
	application.renderer.pickPhysicalDevice();
	application.renderer.createLogicalDevice();
	application.swapchain.createSwapchain();
	application.swapchain.createImageViews();
	application.renderer.createRenderPass();
	application.renderer.createGraphicsPipeline();
	application.swapchain.createFramebuffers();
	application.renderer.createCommandPool();
	application.renderer.createCommandBuffers();
	application.renderer.createSyncObjects();

	std::cout << "Renderer initialized!" << std::endl;
}

void Renderer::createGraphicsPipeline() {
	auto vertexShaderCode = fileSystem.readFile("src/renderer/shaders/vert.spv");
	auto fragmentShaderCode = fileSystem.readFile("src/renderer/shaders/frag.spv");

	VkShaderModule vertexShaderModule = application.renderer.createShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = application.renderer.createShaderModule(fragmentShaderCode);

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

	VkResult pipelineLayoutResult = vkCreatePipelineLayout(application.renderer.device, &pipelineLayoutCreateInfo, nullptr, &application.renderer.pipelineLayout);

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
	graphicsPipelineCreateInfo.layout = application.renderer.pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = application.renderer.renderPass;
	graphicsPipelineCreateInfo.subpass = 0;

	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;

	VkResult graphicsPipelineResult = vkCreateGraphicsPipelines(application.renderer.device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &application.renderer.graphicsPipeline);

	if (graphicsPipelineResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipeline!");
	}
	else {
		std::cout << "Successfully created graphics pipeline!" << std::endl;
	}

	vkDestroyShaderModule(application.renderer.device, vertexShaderModule, nullptr);
	vkDestroyShaderModule(application.renderer.device, fragmentShaderModule, nullptr);
}

VkShaderModule Renderer::createShaderModule(const std::vector<char>& shaderCode) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	VkShaderModule shaderModule;

	VkResult result = vkCreateShaderModule(application.renderer.device, &createInfo, nullptr, &shaderModule);

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
	colorAttachment.format = application.swapchain.getImageFormat();
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

	VkResult result = vkCreateRenderPass(application.renderer.device, &renderPassCreateInfo, nullptr, &application.renderer.renderPass);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create render pass!");
	}
	else {
		std::cout << "Successfully created render pass!" << std::endl;
	}
}

void Renderer::createCommandPool() {
	Renderer::queueFamilyIndices queueFamilyIndices = application.renderer.findQueueFamilies(application.renderer.physicalDevice);

	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VkResult commandPoolResult = vkCreateCommandPool(application.renderer.device, &commandPoolCreateInfo, nullptr, &application.renderer.commandPool);

	if (commandPoolResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}
	else {
		std::cout << "Successfully created command pool!" << std::endl;
	}
}

void Renderer::createCommandBuffers() {
	application.renderer.commandBuffers.resize(application.renderer.MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = application.renderer.commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = (uint32_t)application.renderer.commandBuffers.size();

	VkResult commandBufferResult = vkAllocateCommandBuffers(application.renderer.device, &commandBufferAllocateInfo, application.renderer.commandBuffers.data());

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
	renderPassBeginInfo.renderPass = application.renderer.renderPass;
	renderPassBeginInfo.framebuffer = application.swapchain.getFramebuffers()[imageIndex];
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = application.swapchain.getExtent();

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, application.renderer.graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(application.swapchain.getExtent().width);
	viewport.height = static_cast<float>(application.swapchain.getExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = application.swapchain.getExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	VkResult commandBufferResult = vkEndCommandBuffer(commandBuffer);

	if (commandBufferResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	};
}

void Renderer::createSyncObjects() {
	application.renderer.imageAvailableSemaphores.resize(application.renderer.MAX_FRAMES_IN_FLIGHT);
	application.renderer.renderFinishedSemaphores.resize(application.renderer.MAX_FRAMES_IN_FLIGHT);
	application.renderer.inFlightFences.resize(application.renderer.MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	for (size_t i = 0; i < application.renderer.MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult imageAvailableSemaphoreResult = vkCreateSemaphore(application.renderer.device, &semaphoreCreateInfo, nullptr, &application.renderer.imageAvailableSemaphores[i]);
		VkResult renderFinishedSemaphoreResult = vkCreateSemaphore(application.renderer.device, &semaphoreCreateInfo, nullptr, &application.renderer.renderFinishedSemaphores[i]);
		VkResult inFlightFenceResult = vkCreateFence(application.renderer.device, &fenceCreateInfo, nullptr, &inFlightFences[i]);

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

	vkDeviceWaitIdle(application.renderer.device);

	application.swapchain.cleanup();

	vkDestroyPipeline(application.renderer.device, application.renderer.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(application.renderer.device, application.renderer.pipelineLayout, nullptr);

	vkDestroyRenderPass(application.renderer.device, application.renderer.renderPass, nullptr);

	for (size_t i = 0; i < application.renderer.MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(application.renderer.device, application.renderer.renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(application.renderer.device, application.renderer.imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(application.renderer.device, application.renderer.inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(application.renderer.device, application.renderer.commandPool, nullptr);

	vkDestroyDevice(application.renderer.device, nullptr);

	if (application.renderer.enableValidationLayers) {
		application.renderer.destroyDebugUtilsMessengerExt(application.renderer.instance, application.renderer.debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(application.renderer.instance, application.renderer.surface, nullptr);
	vkDestroyInstance(application.renderer.instance, nullptr);

	std::cout << "Renderer cleaned up!" << std::endl;
}

void Renderer::drawFrame() {
	vkWaitForFences(application.renderer.device, 1, &application.renderer.inFlightFences[application.renderer.currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult acquireNextImageResult = vkAcquireNextImageKHR(application.renderer.device, application.swapchain.getSwapchain(), UINT64_MAX, application.renderer.imageAvailableSemaphores[application.renderer.currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR || acquireNextImageResult == VK_SUBOPTIMAL_KHR || application.renderer.framebufferResized) {
		application.renderer.framebufferResized = false;
		application.swapchain.recreateSwapchain();

		std::cout << "Swap chain out of date, recreating..." << std::endl;

		return;
	}
	else if (acquireNextImageResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	vkResetFences(application.renderer.device, 1, &application.renderer.inFlightFences[application.renderer.currentFrame]);

	vkResetCommandBuffer(application.renderer.commandBuffers[currentFrame], 0);
	application.renderer.recordCommandBuffer(application.renderer.commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { application.renderer.imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &application.renderer.commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { application.renderer.renderFinishedSemaphores[currentFrame]};
	
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult queueSubmitResult = vkQueueSubmit(application.renderer.graphicsQueue, 1, &submitInfo, application.renderer.inFlightFences[currentFrame]);

	if (queueSubmitResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { application.swapchain.getSwapchain()};
	
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(application.renderer.graphicsQueue, &presentInfo);

	application.renderer.currentFrame = (application.renderer.currentFrame + 1) % application.renderer.MAX_FRAMES_IN_FLIGHT;
}

void Renderer::createInstance() {
	if (application.renderer.enableValidationLayers && !application.renderer.checkValidationLayerSupport()) {
		throw std::runtime_error("Validation layers requested, but not supported!");
	}
	else {
		std::cout << "Validation layers enabled!" << std::endl;
	}

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	applicationInfo.pApplicationName = application.window.getWindowName();
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

	applicationInfo.pEngineName = "None";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	applicationInfo.apiVersion = VK_API_VERSION_1_4;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	auto extensions = application.renderer.getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (application.renderer.enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = application.renderer.validationLayers.data();

		application.renderer.populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &application.renderer.instance) != VK_SUCCESS) {
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

	for (const char* layerName : application.renderer.validationLayers) {
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
	application.renderer.glfwExtensions = glfwGetRequiredInstanceExtensions(&application.renderer.glfwExtensionCount);

	std::vector<const char*> extensions(application.renderer.glfwExtensions, application.renderer.glfwExtensions + application.renderer.glfwExtensionCount);

	if (application.renderer.enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void Renderer::setupDebugMessenger() {
	if (!application.renderer.enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};

	application.renderer.populateDebugMessengerCreateInfo(createInfo);

	if (application.renderer.createDebugUtilsMessengerExt(application.renderer.instance, &createInfo, nullptr, &application.renderer.debugMessenger) != VK_SUCCESS) {
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
	createInfo.pfnUserCallback = application.renderer.debugCallback;
}

void Renderer::pickPhysicalDevice() {
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(application.renderer.instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPU with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(application.renderer.instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (application.renderer.isPhysicalDeviceSuitable(device)) {
			application.renderer.setPhysicalDevice(device);

			std::cout << "Successfully picked physical device!" << std::endl;

			break;
		}
	}

	if (application.renderer.getPhysicalDevice() == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable physical device!");
	}
}

bool Renderer::isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice) {
	application.renderer.indices = application.renderer.findQueueFamilies(physicalDevice);

	bool extensionsSupported = application.renderer.checkPhysicalDeviceExtensionSupport(physicalDevice);

	bool swapChainAdequate = false;
	
	if (extensionsSupported) {
		Swapchain::swapchainSupportDetails swapchainSupport = application.swapchain.querySwapchainSupport(physicalDevice);
		swapChainAdequate = !swapchainSupport.surfaceFormats.empty() && !swapchainSupport.presentModes.empty();
	}

	return application.renderer.indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Renderer::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
	uint32_t extensionCount;

	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
	
	std::set<std::string> requiredExtensions(application.renderer.deviceExtensions.begin(), application.renderer.deviceExtensions.end());

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
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, application.renderer.surface, &presentSupport);

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