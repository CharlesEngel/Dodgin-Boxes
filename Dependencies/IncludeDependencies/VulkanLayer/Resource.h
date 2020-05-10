#pragma once

#include "Buffer.h"
#include "Texture.h"
#include "Pipeline.h"

#include <vector>

struct VulkanResource
{
	VkDevice device;
	VulkanPipeline pipeline;
	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> descriptor_sets;
	std::vector<VulkanBuffer> uniform_buffers;
	std::vector<std::vector<VulkanTexture>> textures;
};

struct VulkanResourceParameters
{
	VulkanPipeline pipeline;
	VulkanDevice device;
	VulkanSwapChain swap_chain;
	std::vector<VulkanBuffer> uniform_buffers;
	std::vector<std::vector<VulkanTexture>> textures;
};

// Initializes values in VulkanResource
void create_resource(VulkanResource &resource, VulkanResourceParameters &parameters);

// Frees memory in VulkanResource
void cleanup_resource(VulkanResource &resource);