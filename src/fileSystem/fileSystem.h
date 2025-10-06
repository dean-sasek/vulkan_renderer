#pragma once
#define filesystem_h

#include <fstream>
#include <vector>
#include <iostream>

class FileSystem {
	public:
		std::vector<char> readFile(const std::string& fileName);
	private:
};

extern FileSystem fileSystem;