#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <vector>
#include <string>

#include "Device.h"

// Contains shader module
struct VulkanShader
{
	VkShaderModule shader_module;
	VkDevice device;
	char type;
};

struct VulkanShaderParameters
{
	VulkanDevice device;
	std::string file;
	char type;
};

// Read shader file
std::vector<char> read_file(std::string filename);

// Initializes values in VulkanShader
void create_shader(VulkanShader &shader, VulkanShaderParameters &parameters);

// Frees memory in VulkanShader
void cleanup_shader(VulkanShader &shader);

// Reads a shader's file
std::vector<char> read_file(std::string filename);