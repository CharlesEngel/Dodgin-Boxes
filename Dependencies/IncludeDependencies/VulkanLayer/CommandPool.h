#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

// Contains a command pool 
struct VulkanCommandPool
{
	VkCommandPool command_pool;
	VkDevice device;
};

struct VulkanCommandPoolParameters
{
	VkDevice device;
	uint32_t graphics_family_index;
};

// Initializes values in VulkanCommandPool
void create_command_pool(VulkanCommandPool &command_pool, VulkanCommandPoolParameters &parameters);

// Frees memory in VulkanCommandPool
void cleanup_command_pool(VulkanCommandPool &command_pool);