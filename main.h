#pragma once
#define main_h

#define NOMINMAX

#include <iostream>
#include <stdexcept>

class Application {
	public:
		void init();
		void loop();
		void cleanup();

		bool running = false;
	private:
};

extern Application _application;