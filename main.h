#pragma once
#define main_h

#define NOMINMAX

#include <stdexcept>

#include "src/window/window.h"
#include "src/renderer/renderer.h"
#include "src/renderer/swapchain.h"
#include "src/logger/logger.h"
#include "src/input/input.h"

class Application {
	public:
		void init();
		void loop();
		void cleanup();

		Renderer renderer;
		Swapchain swapchain;
		Window window;
		Logger logger;
		Input input;

		bool running = false;
	private:
};

extern Application application;