#include "main.h"

#include "window/window.h"
#include "renderer/renderer.h"
#include "input/input.h"
#include "fileSystem/fileSystem.h"

Application _application;

void Application::init() {
	std::cout << "Initializing application..." << std::endl;

	_window.init();
	_renderer.init();

	_application.running = true;

	while (_application.running) {
		_application.loop();
	}
}

void Application::loop() {
	_window.poll();

	if (_window.shouldClose()) {
		_application.cleanup();

		return;
	}

	if (_application.running) {
		_renderer.drawFrame();
	}
}

void Application::cleanup() {
	_application.running = false;

	_window.cleanup();
	_renderer.cleanup();
}

int main() {
	try {
		_application.init();
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