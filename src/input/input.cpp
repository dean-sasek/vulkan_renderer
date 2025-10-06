#include "input.h"

#include "../../main.h"

void Input::loop(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		application.cleanup();
	}
}