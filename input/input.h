#pragma once
#define input_h

#include "GLFW/glfw3.h"
#include "../main.h"

class Input {
	public:
		void loop(GLFWwindow* window);
	private:
};

extern Input _input;