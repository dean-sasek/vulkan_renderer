#include "input.h"
#include "../application/application.h"

void Input::loop(Application& application, GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		application.cleanup(application);
	}
}