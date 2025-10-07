#include "ui.h"

#include <array>

#include "../application/application.h"
#include "../logger/logger.h"

void UI::init(Application& application) {
	log_info("Initializing UI...");

	this->application = &application;

	createUIPipeline();

	createVertexBuffer(1000 * sizeof(vertex2D));

	log_info("UI initialized!");
}

void UI::drawUI() {
	application->ui.clearVertices();

	application->ui.drawBox(500, 200, 300, 50);
}

void UI::clearVertices() {
	vertices.clear();
}

void UI::drawBox(float x, float y, float width, float height) {
	glm::vec3 color = {0.0f, 1.0f, 0.0f};

	vertex2D topLeft = { {x, y}, color };
	vertex2D topRight = { {x + width, y}, color };
	vertex2D bottomLeft = { {x, y + height}, color };
	vertex2D bottomRight = { {x + width, y + height}, color };

	vertices.push_back(topLeft);
	vertices.push_back(bottomLeft);
	vertices.push_back(topRight);

	vertices.push_back(topRight);
	vertices.push_back(bottomLeft);
	vertices.push_back(bottomRight);
}

void UI::createVertexBuffer(size_t bufferSize) {
	VkDevice device = application->renderer.getDevice();

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSize;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult bufferResult = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &vertexBuffer);

	if (bufferResult != VK_SUCCESS) {
		log_error("Failed to create vertex buffer!");
	}
	else {
		log_info("Successfully created vertex buffer!");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = application->renderer.findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkResult memoryResult = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &vertexBufferMemory);

	if (memoryResult != VK_SUCCESS) {
		log_error("Failed to allocate vertex buffer memory!")
	}
	else {
		log_info("Successfully allocated vertex buffer memory!");
	}

	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
}

void UI::updateVertexBuffer() {
	VkDevice device = application->renderer.getDevice();

	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, vertices.size() * sizeof(vertex2D), 0, &data);
	memcpy(data, vertices.data(), vertices.size() * sizeof(vertex2D));
	vkUnmapMemory(device, vertexBufferMemory);
}

void UI::render(VkCommandBuffer commandBuffer) {
	if (vertices.empty()) {
		return;
	}

	updateVertexBuffer();

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, uiPipeline);

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
}

void UI::createUIPipeline() {
	log_info("Creating UI pipeline...");

	Pipelines::pipelineStructure pipelineStructure{};

	pipelineStructure.vertexShaderPath = "src/renderer/shaders/ui_vert.spv";
	pipelineStructure.fragmentShaderPath = "src/renderer/shaders/ui_frag.spv";

	static VkVertexInputBindingDescription vertexInputBindingDescription{};
	vertexInputBindingDescription.binding = 0;
	vertexInputBindingDescription.stride = sizeof(vertex2D);
	vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	static std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributeDescriptions{};
	vertexInputAttributeDescriptions[0].binding = 0;
	vertexInputAttributeDescriptions[0].location = 0;
	vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributeDescriptions[0].offset = offsetof(vertex2D, position);

	vertexInputAttributeDescriptions[1].binding = 0;
	vertexInputAttributeDescriptions[1].location = 1;
	vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributeDescriptions[1].offset = offsetof(vertex2D, position);

	pipelineStructure.vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pipelineStructure.vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	pipelineStructure.vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
	pipelineStructure.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
	pipelineStructure.vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

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

	VkPipelineLayout pipelineLayout = application->pipelines.createPipelineLayout();
	uiPipelineLayout = pipelineLayout;

	pipelineStructure.depthStencilStateCreateInfo = nullptr;

	pipelineStructure.pipelineLayout = pipelineLayout;
	pipelineStructure.renderPass = application->renderer.getRenderPass();
	pipelineStructure.subpass = 0;

	VkPipeline pipeline = application->pipelines.createPipeline(pipelineStructure);
	uiPipeline = pipeline;

	log_info("Successfully created UI pipeline!");
}

void UI::cleanup() {
	log_info("Cleaning up UI...");

	log_info("UI cleaned up!");
}