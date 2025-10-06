#include "application.h"

void Application::init(Application& application) {
	log_info("Initializing application...");

	application.window.init(application);
	application.renderer.init(application);

	application.running = true;

	while (application.running) {
		application.loop(application);
	}
}

void Application::loop(Application& application) {
	application.window.poll(application);

	if (application.window.shouldClose(application)) {
		application.cleanup(application);

		return;
	}

	if (application.running) {
		application.renderer.drawFrame(application);
	}
}

void Application::cleanup(Application& application) {
	application.running = false;

	application.window.cleanup(application);
	application.renderer.cleanup(application);
}