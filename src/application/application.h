#pragma once
#define application_h

#include "../../src/window/window.h"
#include "../../src/renderer/renderer.h"
#include "../../src/renderer/swapchain.h"
#include "../../src/logger/logger.h"
#include "../../src/input/input.h"
#include "../../src/file_system/file_system.h"

class Application {
	public:
		void init(Application& application);
		void loop(Application& application);
		void cleanup(Application& application);

		Renderer renderer;
		Swapchain swapchain;
		Window window;
		Logger logger;
		Input input;

		bool running = false;
	private:
};