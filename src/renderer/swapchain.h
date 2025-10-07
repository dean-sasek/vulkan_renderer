#pragma once
#define swapchain_h

#include <vector>
#include <iostream>

#include "vulkan/vulkan.h"

class Application;

class Swapchain {
public:
	void init(Application& application);

	void cleanup();

	void createSwapchain();
	void recreateSwapchain();

	void createImageViews();
	void createFramebuffers();

	const VkFormat getImageFormat();
	VkExtent2D getExtent();
	std::vector<VkFramebuffer> getFramebuffers();
	const VkSwapchainKHR getSwapchain();

	struct swapchainSupportDetails {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	swapchainSupportDetails querySwapchainSupport(Application& application, VkPhysicalDevice physicalDevice);
private:
	Application* application = nullptr;
	void setApplication(Application& application);

	VkSwapchainKHR swapchain;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;

	VkFormat imageFormat;
	VkExtent2D imageExtent;

	VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);
};