#pragma once
#define application_h

#include "../../src/window/window.h"
#include "../../src/renderer/renderer.h"
#include "../../src/logger/logger.h"
#include "../../src/input/input.h"
#include "../../src/file_system/file_system.h"
#include "../../src/ui/ui.h"

class Application {
	public:
		void init();
		void loop();
		void cleanup();

		Renderer renderer;
		Swapchain swapchain;
		Pipelines pipelines;
		Window window;
		Logger logger;
		Input input;
		UI ui;

		bool running = false;
	private:
};