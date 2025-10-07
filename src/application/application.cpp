#include "application.h"

void Application::init() {
	log_info("Initializing application...");

	window.init(*this);
	renderer.init(*this);
	ui.init(*this);

	log_info("Application initialized!");

	running = true;

	while (running) {
		loop();
	}
}

void Application::loop() {
	window.poll(*this);

	if (window.shouldClose(*this)) {
		cleanup();

		return;
	}

	if (running) {
		renderer.drawFrame();
	}
}

void Application::cleanup() {
	running = false;

	window.cleanup(*this);
	renderer.cleanup();
}