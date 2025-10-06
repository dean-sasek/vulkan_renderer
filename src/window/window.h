#pragma once
#define window_h

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

class Window {
	public:
		void init();
		void poll();
		void cleanup() const;
		bool shouldClose();

		const char* getWindowName();
		const uint32_t getWindowWidth();
		const uint32_t getWindowHeight();

		void setWindowName(char* windowName);
		void setWindowWidth(uint32_t windowWidth);
		void setWindowHeight(uint32_t windowHeight);
		void setWindowSize(uint32_t windowWidth, uint32_t windowHeight);

		GLFWwindow* glfwWindow = nullptr;
	private:
		const char* windowName = "dean";
		uint32_t windowWidth = 800;
		uint32_t windowHeight = 600;

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};