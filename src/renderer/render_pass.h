#pragma once
#define render_pass_h

#include <vulkan/vulkan.h>

class Application;

class RenderPass {
	public:
		struct renderPassStructure {
			VkAttachmentDescription attachmentDescription;
			VkAttachmentReference attachmentReference;
			VkSubpassDescription subpassDescription;
			VkSubpassDependency subpassDependency;
			VkRenderPassCreateInfo renderPassCreateInfo;
		};

		void init(Application& application);
		VkRenderPass createRenderPass(RenderPass::renderPassStructure renderPassStructure);
	private:
		Application* application;
};