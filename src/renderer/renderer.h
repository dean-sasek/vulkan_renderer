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
		void cleanup(Application& application);
		
		VkRenderPass getRenderPass(Application& application);

		void drawFrame(Application& application);

		VkSurfaceKHR surface;
		bool framebufferResized = false;

		Swapchain swapchain;

		VkPhysicalDevice getPhysicalDevice(Application& application);
		VkDevice getDevice(Application& application);

		struct queueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			const bool isComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		queueFamilyIndices indices;
		queueFamilyIndices findQueueFamilies(Application& application, VkPhysicalDevice physicalDevice);
	private:
		const int MAX_FRAMES_IN_FLIGHT = 2;

		VkDevice device;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		bool checkPhysicalDeviceExtensionSupport(Application& application, VkPhysicalDevice physicalDevice);

		void createInstance(Application& application);
		void createSurface(Application& application);

		bool checkValidationLayerSupport(Application& application);
		std::vector<const char*> getRequiredExtensions(Application& application);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void setupDebugMessenger(Application& application);
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		VkResult createDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void destroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		void populateDebugMessengerCreateInfo(Application& application, VkDebugUtilsMessengerCreateInfoEXT createInfo);

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = 0;

		const std::vector<const char*> validationLayers;
		bool enableValidationLayers = true;

		VkInstance instance = VK_NULL_HANDLE;

		void pickPhysicalDevice(Application& application);
		bool isPhysicalDeviceSuitable(Application& application, VkPhysicalDevice physicalDevice);

		void createLogicalDevice(Application& application);

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		void setPhysicalDevice(Application& application, VkPhysicalDevice physicalDevice);

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

		void createGraphicsPipeline(Application& application);

		VkShaderModule createShaderModule(Application& application, const std::vector<char>& shaderCode);
	
		//VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		//VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		//VkExtent2D chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

		VkRenderPass renderPass;
		VkPipeline graphicsPipeline;
		VkPipelineLayout pipelineLayout;
		void createRenderPass(Application& application);

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		void createCommandPool(Application& application);
		void createCommandBuffers(Application& application);
		void recordCommandBuffer(Application& application, VkCommandBuffer commandBuffer, uint32_t imageIndex);

		uint32_t currentFrame = 0;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		void createSyncObjects(Application& application);
};