#include "logger.h"

void Logger::log(Logger::level level, const std::string& message) {
	std::ostringstream logStream;

	logStream << "[" << getDateAndTime() << "] "
			  << "[" << levelToString(level) << "] "
			  << message << std::endl;
	std::cout << logStream.str();
}

std::string Logger::getDateAndTime() {
	auto now = std::chrono::system_clock::now();
	std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

	std::tm localTime;
	localtime_s(&localTime, &nowTime);

	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);

	return std::string(buffer);
}

std::string Logger::levelToString(Logger::level level) {
	switch (level) {
		case Logger::level::info:
			return "info";
		case Logger::level::warning:
			return "warning";
		case Logger::level::error:
			return "error";
		default:
			return "unknown";
	}
}