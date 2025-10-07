#pragma once
#define pipelines_h

#include <unordered_map>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

class Application;

class Pipelines {
	public:
		void init(Application& application);

		struct pipelineStructure {
			std::string vertexShaderPath;
			std::string fragmentShaderPath;

			VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
			VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
			VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
			VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
			VkPipelineColorBlendAttachmentState colorBlendAttachmentStateCreateInfo;
			VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
			VkPipelineDepthStencilStateCreateInfo* depthStencilStateCreateInfo;
			VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;

			VkPipelineLayout pipelineLayout;
			VkRenderPass renderPass;
			uint32_t subpass;
			
			std::vector<VkDynamicState> dynamicStates;
		};

		VkPipeline createPipeline(const pipelineStructure pipelineStructure);
		VkPipelineLayout createPipelineLayout();

		void destroyPipeline(VkPipeline pipeline);
	private:
		Application* application = nullptr;
};