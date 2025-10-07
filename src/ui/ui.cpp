#include "ui.h"

#include "../application/application.h"
#include "../logger/logger.h"

void UI::init(Application& application) {
	log_info("Initializing UI...");

	this->application = &application;

	log_info("UI initialized!");
}

void UI::cleanup() {
	log_info("Cleaning up UI...");

	log_info("UI cleaned up!");
}