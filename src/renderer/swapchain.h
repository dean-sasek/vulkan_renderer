#pragma once
#define swapchain_h

#include <vector>
#include <iostream>

#include "vulkan/vulkan.h"
#include "../window/window.h"

class Swapchain {
public:
	void cleanup();

	void createSwapchain();
	void recreateSwapchain();

	void createImageViews();
	void createFramebuffers();

	VkFormat getImageFormat();
	VkExtent2D getExtent();
	std::vector<VkFramebuffer> getFramebuffers();
	VkSwapchainKHR getSwapchain();

	struct swapchainSupportDetails {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	swapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physicalDevice);
private:
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