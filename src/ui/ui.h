#pragma once
#define ui_h

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

class Application;

class UI {
	public:
		void init(Application& application);
		void cleanup();
	private:
		Application* application = nullptr;

		void createUIPipeline();

		VkPipeline uiPipeline;
		VkPipelineLayout uiPipelineLayout;
};