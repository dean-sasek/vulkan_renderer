#pragma once
#define input_h

#include "GLFW/glfw3.h"

class Application;

class Input {
	public:
		void loop(Application& application, GLFWwindow* window);
	private:
};