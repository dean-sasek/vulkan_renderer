#include "shaders.h"
#include "../../application/application.h"

void Shaders::init(Application& application) {
	log_info("Initializing shaders...");

	this->application = &application;

	log_info("Shaders initialized!");
}

void Shaders::poll() {

}

void Shaders::compileShader(const std::string& glslcPath, const std::string& filePath, const std::string& ouputPath) {

}

VkShaderModule Shaders::createShaderModule(const std::vector<char>& shaderCode) {
	log_info("Creating shader module...");

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	VkShaderModule shaderModule;

	VkResult result = vkCreateShaderModule(application->renderer.getDevice(), &createInfo, nullptr, &shaderModule);

	if (result != VK_SUCCESS) {
		log_error("Failed to create shader module!");
	}
	else {
		log_info("Successfully created shader module!");

		return shaderModule;
	}
}

void Shaders::destroyShaderModule(VkShaderModule shaderModule) {
	log_info("Destroying shader module...");

	vkDestroyShaderModule(application->renderer.getDevice(), shaderModule, nullptr);

	log_info("Shader module destroyed!");
}