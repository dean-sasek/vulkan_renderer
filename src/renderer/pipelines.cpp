#include "pipelines.h"
#include "../application/application.h"

void Pipelines::init(Application& application) {
	log_info("Initializing pipelines...");

	this->application = &application;

	log_info("Pipelines initialized!");
}

VkPipeline Pipelines::createPipeline(const pipelineStructure pipelineStructure) {
	log_info("Creating pipeline...");
	
	auto vertexShaderCode = fileSystem.readFile(pipelineStructure.vertexShaderPath);
	auto fragmentShaderCode = fileSystem.readFile(pipelineStructure.fragmentShaderPath);

	VkShaderModule vertexShaderModule = application->renderer.createShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = application->renderer.createShaderModule(fragmentShaderCode);

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


	VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.scissorCount = 1;

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.stageCount = 2;
	graphicsPipelineCreateInfo.pStages = shaderStages;
	graphicsPipelineCreateInfo.pVertexInputState = &pipelineStructure.vertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineStructure.inputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipelineStructure.rasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &pipelineStructure.multisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = pipelineStructure.depthStencilStateCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &pipelineStructure.colorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = &pipelineStructure.dynamicStateCreateInfo;
	graphicsPipelineCreateInfo.layout = pipelineStructure.pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = pipelineStructure.renderPass;
	graphicsPipelineCreateInfo.subpass = pipelineStructure.subpass;

	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;

	VkPipeline pipeline;

	VkResult graphicsPipelineResult = vkCreateGraphicsPipelines(application->renderer.getDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);

	if (graphicsPipelineResult != VK_SUCCESS) {
		log_error("Failed to create pipeline!");
	}
	else {
		log_info("Successfully created pipeline!");
	}

	vkDestroyShaderModule(application->renderer.getDevice(), vertexShaderModule, nullptr);
	vkDestroyShaderModule(application->renderer.getDevice(), fragmentShaderModule, nullptr);

	return pipeline;
}

void Pipelines::destroyPipeline(VkPipeline pipeline) {
	log_info("Destroying pipeline...");

	vkDestroyPipeline(application->renderer.getDevice(), pipeline, nullptr);

	log_info("Pipeline destroyed!");
}