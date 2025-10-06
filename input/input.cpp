#include "input.h"

#include <iostream>

Input _input;

void Input::loop(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_application.cleanup();
	}
}