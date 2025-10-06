#include "file_system.h"
#include "../logger/logger.h"
#include "../../main.h"

FileSystem fileSystem;

std::vector<char> FileSystem::readFile(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		log_error("Failed to open file: " + fileName);
	}
	else {
		log_info("Successfully opened file: " + fileName);
	}

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}