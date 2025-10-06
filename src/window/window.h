#pragma once
#define window_h

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

class Application;

class Window {
	public:
		void init(Application& application);
		void poll(Application& application);
		void cleanup(Application& application) const;
		bool shouldClose(Application& application);

		const char* getWindowName(Application& application);
		const uint32_t getWindowWidth(Application& application);
		const uint32_t getWindowHeight(Application& application);

		void setWindowName(Application& application, char* windowName);
		void setWindowWidth(Application& application, uint32_t windowWidth);
		void setWindowHeight(Application& application, uint32_t windowHeight);
		void setWindowSize(Application& application, uint32_t windowWidth, uint32_t windowHeight);

		GLFWwindow* glfwWindow = nullptr;
	private:
		const char* windowName = "Vulkan 1.4";
		uint32_t windowWidth = 800;
		uint32_t windowHeight = 600;

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};