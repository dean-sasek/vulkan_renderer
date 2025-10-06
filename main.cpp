#include "main.h"

#include "src/window/window.h"
#include "src/renderer/renderer.h"
#include "src/input/input.h"
#include "src/file_system/file_system.h"

Application application;

void Application::init() {
	log_info("Initializing application...");

	application.window.init();
	application.renderer.init();

	application.running = true;

	while (application.running) {
		application.loop();
	}
}

void Application::loop() {
	application.window.poll();

	if (application.window.shouldClose()) {
		application.cleanup();

		return;
	}

	if (application.running) {
		application.renderer.drawFrame();
	}
}

void Application::cleanup() {
	application.running = false;

	application.window.cleanup();
	application.renderer.cleanup();
}

int main() {
	try {
		application.init();
	}
	catch (const std::runtime_error& e) {
		log_error(e.what());
	}
	catch (...) {
		log_error("Unknown error!");
	}

	log_info("Application ended successfully!");

	return 0;
}