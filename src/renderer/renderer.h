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

#define VK_USE_PLATFORM_WIN32_KHR

class Renderer {
	public:
		void init(Application& application);
		void cleanup();
		
		VkRenderPass getRenderPass();

		void drawFrame();

		VkSurfaceKHR surface;
		bool framebufferResized = false;

		Swapchain swapchain;

		VkPhysicalDevice getPhysicalDevice();
		VkDevice getDevice();

		struct queueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			const bool isComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		queueFamilyIndices indices;
		queueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
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

		/*
		void createSwapChain();
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<VkImageView> swapChainImageViews;
		void createImageViews();
		std::vector<VkFramebuffer> swapChainFramebuffers;
		void createFramebuffers();
		void recreateSwapChain();
		void cleanupSwapChain();
		*/

		void createGraphicsPipeline();

		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);
	
		//VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		//VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		//VkExtent2D chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

		VkRenderPass renderPass;
		VkPipeline graphicsPipeline;
		VkPipelineLayout pipelineLayout;
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