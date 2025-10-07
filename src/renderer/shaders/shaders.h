#pragma once
#define shaders_h

#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <filesystem>

class Shaders {
	public:
		void init();
		void poll();

		void compileShader(const std::string& glslcPath, const std::string& filePath, const std::string& ouputPath);
		void compileShaders(std::vector<std::string>& shaderPaths);

		void createShaderModule(auto shaderCode);
	private:

};