#include "window.h"

#include "../input/input.h"
#include "../renderer/renderer.h"
#include "../../main.h"

void Window::init() {
	log_info("Initializing window...");

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	application.window.glfwWindow = glfwCreateWindow(application.window.getWindowWidth(), application.window.getWindowHeight(), application.window.getWindowName(), nullptr, nullptr);
	glfwSetWindowUserPointer(application.window.glfwWindow, this);
	glfwSetFramebufferSizeCallback(application.window.glfwWindow, application.window.framebufferResizeCallback);

	log_info("Window initialized!");
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	log_info("Framebuffer resized!");

	auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	renderer->framebufferResized = true;
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(application.window.glfwWindow);
}

void Window::poll() {
	glfwPollEvents();

	application.input.loop(application.window.glfwWindow);
}

void Window::cleanup() const {
	log_info("Cleaning up window...");

	glfwDestroyWindow(application.window.glfwWindow);
	glfwTerminate();

	log_info("Window cleaned up!");
}

const char* Window::getWindowName() {
	return application.window.windowName;
}

const uint32_t Window::getWindowWidth() {
	return application.window.windowWidth;
}

const uint32_t Window::getWindowHeight() {
	return application.window.windowHeight;
}

void Window::setWindowName(char* windowName) {
	application.window.windowName = windowName;

	glfwSetWindowTitle(application.window.glfwWindow, application.window.windowName);
}

void Window::setWindowWidth(uint32_t windowWidth) {
	application.window.windowWidth = windowWidth;

	glfwSetWindowSize(application.window.glfwWindow, application.window.getWindowWidth(), application.window.getWindowHeight());
}

void Window::setWindowHeight(uint32_t windowHeight) {
	application.window.windowHeight = windowHeight;

	glfwSetWindowSize(application.window.glfwWindow, application.window.getWindowWidth(), application.window.getWindowHeight());
}

void Window::setWindowSize(uint32_t windowWidth, uint32_t windowHeight) {
	application.window.setWindowWidth(windowWidth);
	application.window.setWindowHeight(windowHeight);

	glfwSetWindowSize(application.window.glfwWindow, application.window.getWindowWidth(), application.window.getWindowHeight());

	log_info("Window resized!");
}