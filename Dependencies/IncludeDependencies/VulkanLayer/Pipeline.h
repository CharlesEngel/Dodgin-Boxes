#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <array>

#include "Device.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"

enum PipelineFlags
{
	PIPELINE_BLEND_ENABLE = 1,
	PIPELINE_BACKFACE_CULL_DISABLE = 2,
	PIPELINE_DEPTH_TEST_DISABLE = 3,
	PIPELINE_DEPTH_WRITE_DISABLE = 4
};

struct VulkanPipelineBarrier
{
	VkPipelineStageFlags src;
	VkPipelineStageFlags dst;
	// TODO: Do I want to store dependency flags?
	std::vector<VulkanBuffer> buffers;
	std::vector <VulkanTexture> images;
	VkImageSubresourceRange subresource_range;
};

// Contains a pipeline along with its uniform buffers, textures, and descriptor sets
struct VulkanPipeline
{
	VkDevice device;
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;

	VkDescriptorSetLayout descriptor_set_layout;
	std::vector<VulkanPipelineBarrier> pipeline_barriers;
};

struct VulkanPipelineParameters
{
	VulkanDevice device;
	VulkanSwapChain swap_chain;
	VulkanRenderPass render_pass;
	GLFWwindow *glfw_window;

	std::vector<VulkanShader> shaders;
	std::vector<VkVertexInputBindingDescription> binding_descriptions;
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
	uint32_t num_textures;
	
	std::vector<VulkanPipelineBarrier> pipeline_barriers;

	uint32_t viewport_width;
	uint32_t viewport_height;
	int32_t viewport_offset_x;
	int32_t viewport_offset_y;

	PipelineFlags pipeline_flags;
};

// Initializes values in VulkanPipeline
void create_pipeline(VulkanPipeline &pipeline, VulkanPipelineParameters &parameters);

// Frees memory in VulkanPipeline
void cleanup_pipeline(VulkanPipeline &pipeline);