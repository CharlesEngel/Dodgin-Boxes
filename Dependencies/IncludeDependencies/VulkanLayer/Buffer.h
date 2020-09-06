#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <vector>

#include "Device.h"
#include "MemoryManager.h"

// Contains a buffer, can serve any purpose
struct VulkanBuffer
{
	VkBuffer buffer;
	VulkanMemory buffer_memory;
	VkDevice device;
	VulkanMemoryManager *memory_manager;
	VkDeviceSize size;
	uint32_t range;
	VkBufferUsageFlags usage;
};

struct VulkanBufferParameters
{
	VulkanDevice device;
	VulkanMemoryManager *memory_manager;
	VkDeviceSize size;
	VkBufferUsageFlags usage;
	VkMemoryPropertyFlags properties;
	uint32_t range;
	void *data;
};

struct VulkanBufferDataParameters
{
	void *data;
	uint32_t size;
	VulkanDevice device;
	VkDeviceSize offset;
};

// Initializes values in VulkanBuffer
void create_buffer(VulkanBuffer &buffer, VulkanBufferParameters &parameters);

// Frees memory in VulkanBuffer
void cleanup_buffer(VulkanBuffer &buffer);

void copy_data_one_time(VulkanBuffer &buffer, VulkanBufferDataParameters data);

void copy_data_visible_buffer(VulkanBuffer &buffer, VulkanBufferDataParameters data);