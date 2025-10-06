#include "main.h"
#include "src/application/application.h"

int main() {
	Application application;

	try {
		application.init(application);
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