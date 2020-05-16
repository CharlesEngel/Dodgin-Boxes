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

struct VulkanPipelineBarrier
{
	VkPipelineStageFlags src;
	VkPipelineStageFlags dst;
	// TODO: Do I want to store dependency flags?

	// TODO: figure out this mess

	//uint32_t memory_barrier_count;
	//VkMemoryBarrier *memory_barriers;
	//uint32_t buffer_barrier_count;
	//VkBufferMemoryBarrier *buffer_barriers;
	std::vector<VulkanBuffer> buffers;
	//uint32_t image_barrier_count;
	//VkImageMemoryBarrier *image_barriers;
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
	/*VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> descriptor_sets;*/

	/*std::vector<VulkanBuffer> uniform_buffers;
	std::vector<std::vector<VulkanTexture>> textures;*/

	std::vector<VulkanPipelineBarrier> pipeline_barriers;
};

struct VulkanPipelineParameters
{
	VulkanDevice device;
	VulkanSwapChain swap_chain;
	VulkanRenderPass render_pass;
	GLFWwindow *glfw_window;

	std::vector<VulkanShader> shaders;
	/*std::vector<VulkanBuffer> uniform_buffers;
	std::vector<std::vector<VulkanTexture>> textures;*/
	std::vector<VkVertexInputBindingDescription> binding_descriptions;
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
	uint32_t num_textures;
	
	std::vector<VulkanPipelineBarrier> pipeline_barriers;

	uint32_t viewport_width;
	uint32_t viewport_height;
	int32_t viewport_offset_x;
	int32_t viewport_offset_y;

};

// Contains information about a vertex
// TODO: Should I make it so information is passed in instead of being defined here?

// Initializes values in VulkanPipeline
void create_pipeline(VulkanPipeline &pipeline, VulkanPipelineParameters &parameters);

// Frees memory in VulkanPipeline
void cleanup_pipeline(VulkanPipeline &pipeline);