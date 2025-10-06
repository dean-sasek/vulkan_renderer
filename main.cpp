#include "main.h"

#include "src/window/window.h"
#include "src/renderer/renderer.h"
#include "src/input/input.h"
#include "src/fileSystem/fileSystem.h"

Application application;

void Application::init() {
	std::cout << "Initializing application..." << std::endl;

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
		std::cerr << "Error: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown error!" << std::endl;
	}

	std::cout << "Application ended successfully!" << std::endl;

	return 0;
}