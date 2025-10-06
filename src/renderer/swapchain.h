#pragma once
#define swapchain_h

#include <vector>
#include <iostream>

#include "vulkan/vulkan.h"

class Application;

class Swapchain {
public:
	void cleanup(Application& application);

	void createSwapchain(Application& application);
	void recreateSwapchain(Application& application);

	void createImageViews(Application& application);
	void createFramebuffers(Application& application);

	VkFormat getImageFormat(Application& application);
	VkExtent2D getExtent(Application& application);
	std::vector<VkFramebuffer> getFramebuffers(Application& application);
	VkSwapchainKHR getSwapchain(Application& application);

	struct swapchainSupportDetails {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	swapchainSupportDetails querySwapchainSupport(Application& application, VkPhysicalDevice physicalDevice);
private:
	VkSwapchainKHR swapchain;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;

	VkFormat imageFormat;
	VkExtent2D imageExtent;

	VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapchainExtent(Application& application, const VkSurfaceCapabilitiesKHR& surfaceCapabilities);
};