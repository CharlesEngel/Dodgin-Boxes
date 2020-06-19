#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#include <stb/stb_image.h>

#include <vector>
#include <string>

#include "Device.h"
#include "CommandPool.h"
#include "MemoryManager.h"

enum TextureFlags
{
	TEXTURE_ARRAY = 1,
	TEXTURE_CUBE = 2,
	TEXTURE_SAMPLER_COMPARE_LESS_OR_EQUAL = 4,
	TEXTURE_BORDER_WHITE = 8
};

// Contains a texture and its related information
struct VulkanTextureData
{
	stbi_uc *pixels;
	VkDeviceSize image_size;
	int texture_width, texture_height;
	uint32_t mip_levels;
};

struct VulkanTexture
{
	VulkanMemoryManager *memory_manager;
	VkImage texture_image;
	VulkanMemory texture_memory;
	VkImageView texture_image_view;
	VkSampler texture_sampler;
	VkDevice device;
	VkFormat format;

	uint32_t mip_levels;
	uint32_t height;
	uint32_t width;
};

struct VulkanTextureParameters
{
	VulkanDevice device;
	VulkanMemoryManager *memory_manager;
	VulkanCommandPool command_pool;
	VkImageUsageFlags usage;
	VulkanTextureData data;
	uint32_t height;
	uint32_t width;
	uint32_t layers;
	VkFormat format;
	VkSampleCountFlagBits samples;
	TextureFlags flags;
};

struct VulkanTextureDataParameters
{
	std::string filename;
};

// Initializes values in VulkanTexture
void create_texture(VulkanTexture &texture, VulkanTextureParameters &parameters);

// Frees memory in VulkanTexture
void cleanup_texture(VulkanTexture &texture);

// Load image file into buffer
void load_image(VulkanTextureData &data, VulkanTextureDataParameters &parameters);

void cleanup_image(VulkanTextureData &data);