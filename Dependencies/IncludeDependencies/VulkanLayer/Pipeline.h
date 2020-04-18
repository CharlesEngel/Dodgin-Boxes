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
	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> descriptor_sets;

	std::vector<VulkanBuffer> uniform_buffers;
	std::vector<std::vector<VulkanTexture>> textures;

	std::vector<VulkanPipelineBarrier> pipeline_barriers;
};

struct VulkanPipelineParameters
{
	VulkanDevice device;
	VulkanSwapChain swap_chain;
	VulkanRenderPass render_pass;
	GLFWwindow *glfw_window;

	std::vector<VulkanShader> shaders;
	std::vector<VulkanBuffer> uniform_buffers;
	std::vector<std::vector<VulkanTexture>> textures;
	
	std::vector<VulkanPipelineBarrier> pipeline_barriers;
};

// Contains information about a vertex
// TODO: Should I make it so information is passed in instead of being defined here?
struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texture_coordinate;

	static VkVertexInputBindingDescription get_binding_description() {
		VkVertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_description;
	}

	static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions = {};

		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(Vertex, position);

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(Vertex, color);

		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descriptions[2].offset = offsetof(Vertex, texture_coordinate);

		return attribute_descriptions;
	}

	bool operator==(const Vertex& other) const {
		return position == other.position && color == other.color && texture_coordinate == other.texture_coordinate;
	}
};

// Initializes values in VulkanPipeline
void create_pipeline(VulkanPipeline &pipeline, VulkanPipelineParameters &parameters);

// Frees memory in VulkanPipeline
void cleanup_pipeline(VulkanPipeline &pipeline);