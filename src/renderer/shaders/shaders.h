#pragma once
#define shaders_h

#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <filesystem>

#include <vulkan/vulkan.h>

class Application;

class Shaders {
	public:
		void init(Application& application);
		void poll();

		void compileShader(const std::string& glslcPath, const std::string& filePath, const std::string& ouputPath);

		VkShaderModule createShaderModule(const std::vector<char>& shaderCode);
		void destroyShaderModule(VkShaderModule shaderModule);
	private:
		Application* application;
};