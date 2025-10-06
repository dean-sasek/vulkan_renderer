#include "window.h"

#include "../input/input.h"
#include "../renderer/renderer.h"

Window _window;

void Window::init() {
	std::cout << "Initializing window..." << std::endl;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	_window.glfwWindow = glfwCreateWindow(_window.getWindowWidth(), _window.getWindowHeight(), _window.getWindowName(), nullptr, nullptr);
	glfwSetWindowUserPointer(_window.glfwWindow, this);
	glfwSetFramebufferSizeCallback(_window.glfwWindow, _window.framebufferResizeCallback);

	std::cout << "Window initialized!" << std::endl;
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	std::cout << "Framebuffer resized!" << std::endl;

	auto renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
	renderer->framebufferResized = true;
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(_window.glfwWindow);
}

void Window::poll() {
	glfwPollEvents();

	_input.loop(_window.glfwWindow);
}

void Window::cleanup() const {
	std::cout << "Cleaning up window..." << std::endl;

	glfwDestroyWindow(_window.glfwWindow);
	glfwTerminate();

	std::cout << "Window cleaned up!" << std::endl;
}

const char* Window::getWindowName() {
	return _window.windowName;
}

const uint32_t Window::getWindowWidth() {
	return _window.windowWidth;
}

const uint32_t Window::getWindowHeight() {
	return _window.windowHeight;
}

void Window::setWindowName(char* windowName) {
	_window.windowName = windowName;

	glfwSetWindowTitle(_window.glfwWindow, _window.windowName);
}

void Window::setWindowWidth(uint32_t windowWidth) {
	_window.windowWidth = windowWidth;

	glfwSetWindowSize(_window.glfwWindow, _window.getWindowWidth(), _window.getWindowHeight());
}

void Window::setWindowHeight(uint32_t windowHeight) {
	_window.windowHeight = windowHeight;

	glfwSetWindowSize(_window.glfwWindow, _window.getWindowWidth(), _window.getWindowHeight());
}

void Window::setWindowSize(uint32_t windowWidth, uint32_t windowHeight) {
	_window.setWindowWidth(windowWidth);
	_window.setWindowHeight(windowHeight);

	glfwSetWindowSize(_window.glfwWindow, _window.getWindowWidth(), _window.getWindowHeight());

	std::cout << "Window resized!" << std::endl;
}