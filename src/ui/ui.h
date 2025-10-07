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
		void render(VkCommandBuffer commandBuffer);

		void drawUI();
		void drawBox(float x, float y, float width, float height);

		void createVertexBuffer(size_t bufferSize);
		void updateVertexBuffer();
		void clearVertices();
	private:
		Application* application = nullptr;

		void createUIPipeline();

		VkPipeline uiPipeline;
		VkPipelineLayout uiPipelineLayout;

		struct vertex2D {
			glm::vec2 position;
			glm::vec3 color;
		};

		std::vector<vertex2D> vertices;
		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
};