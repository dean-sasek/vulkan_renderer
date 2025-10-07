#pragma once
#define pipelines_h

#include <unordered_map>
#include <vulkan/vulkan.h>

class Pipelines {
public:
	void init();

	void createPipeline();
	void destroyPipeline();
private:
};