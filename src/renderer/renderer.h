#pragma once
#define renderer_h

#include <vector>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <cstring>

#include "vulkan/vulkan.h"
#include "swapchain.h"
#include "pipelines.h"
#include "shaders/shaders.h"
#include "render_pass.h"

#define VK_USE_PLATFORM_WIN32_KHR

class Renderer {
	public:
		void init(Application& application);
		void cleanup();
		
		const VkRenderPass getRenderPass();

		void drawFrame();

		VkSurfaceKHR surface;
		bool framebufferResized = false;

		Swapchain swapchain;

		const VkPhysicalDevice getPhysicalDevice();
		VkDevice getDevice();
		std::vector<VkCommandBuffer> getCommandBuffers();

		struct queueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			const bool isComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		queueFamilyIndices indices;
		const queueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);


		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);
	private:
		Application* application = nullptr;
		void setApplication(Application& application);

		const int MAX_FRAMES_IN_FLIGHT = 2;

		VkDevice device;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

		void createInstance();
		void createSurface();

		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void setupDebugMessenger();
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		VkResult createDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void destroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT createInfo);

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = 0;

		const std::vector<const char*> validationLayers;
		bool enableValidationLayers = true;

		VkInstance instance = VK_NULL_HANDLE;

		void pickPhysicalDevice();
		bool isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);

		void createLogicalDevice();

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		void setPhysicalDevice(VkPhysicalDevice physicalDevice);

		void createGraphicsPipeline();

		VkRenderPass renderPass;
		VkPipeline graphicsPipeline;
		VkPipelineLayout graphicsPipelineLayout;
		void createRenderPass();

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		void createCommandPool();
		void createCommandBuffers();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		uint32_t currentFrame = 0;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		void createSyncObjects();
};