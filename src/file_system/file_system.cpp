#include "file_system.h"

FileSystem fileSystem;

std::vector<char> FileSystem::readFile(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + fileName);
	}
	else {
		std::cout << "Successfully opened file!" << std::endl;
	}

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}