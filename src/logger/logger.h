#pragma once
#define logger_h

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>

class Logger {
	public:
		enum class level {
			info,
			warning,
			error
		};

		void log(Logger::level level, const std::string& message);
	private:
		static std::string getDateAndTime();
		static std::string levelToString(Logger::level level);
};

extern Logger logger;

#define log_info(message) logger.log(Logger::level::info, message)
#define log_warning(message) logger.log(Logger::level::warning, message)
#define log_error(message) throw std::runtime_error(message);