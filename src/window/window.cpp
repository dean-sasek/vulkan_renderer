#include "window.h"
#include "../application/application.h"

void Window::init(Application& application) {
	log_info("Initializing window...");

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	application.window.glfwWindow = glfwCreateWindow(application.window.getWindowWidth(application), application.window.getWindowHeight(application), application.window.getWindowName(application), nullptr, nullptr);
	glfwSetWindowUserPointer(application.window.glfwWindow, this);
	glfwSetFramebufferSizeCallback(application.window.glfwWindow, application.window.framebufferResizeCallback);

	log_info("Window initialized!");
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	renderer->framebufferResized = true;
}

bool Window::shouldClose(Application& application) {
	return glfwWindowShouldClose(application.window.glfwWindow);
}

void Window::poll(Application& application) {
	glfwPollEvents();

	application.input.loop(application, application.window.glfwWindow);
}

void Window::cleanup(Application& application) const {
	log_info("Cleaning up window...");

	glfwDestroyWindow(application.window.glfwWindow);
	glfwTerminate();

	log_info("Window cleaned up!");
}

const char* Window::getWindowName(Application& application) {
	return application.window.windowName;
}

const uint32_t Window::getWindowWidth(Application& application) {
	return application.window.windowWidth;
}

const uint32_t Window::getWindowHeight(Application& application) {
	return application.window.windowHeight;
}

void Window::setWindowName(Application& application, char* windowName) {
	application.window.windowName = windowName;

	glfwSetWindowTitle(application.window.glfwWindow, application.window.windowName);
}

void Window::setWindowWidth(Application& application, uint32_t windowWidth) {
	application.window.windowWidth = windowWidth;

	glfwSetWindowSize(application.window.glfwWindow, application.window.getWindowWidth(application), application.window.getWindowHeight(application));
}

void Window::setWindowHeight(Application& application, uint32_t windowHeight) {
	application.window.windowHeight = windowHeight;

	glfwSetWindowSize(application.window.glfwWindow, application.window.getWindowWidth(application), application.window.getWindowHeight(application));
}

void Window::setWindowSize(Application& application, uint32_t windowWidth, uint32_t windowHeight) {
	application.window.setWindowWidth(application, windowWidth);
	application.window.setWindowHeight(application, windowHeight);

	glfwSetWindowSize(application.window.glfwWindow, application.window.getWindowWidth(application), application.window.getWindowHeight(application));

	log_info("Window resized!");
}