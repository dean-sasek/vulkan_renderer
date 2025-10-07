#include "ui.h"

#include "../application/application.h"
#include "../logger/logger.h"

void UI::init(Application& application) {
	log_info("Initializing UI...");

	this->application = &application;

	createUIPipeline();

	log_info("UI initialized!");
}

void UI::createUIPipeline() {
	log_info("Creating UI pipeline...");

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