#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
#include <array>

#include "Device.h"
#include "CommandPool.h"
#include "Device.h"
#include "CommandPool.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Resource.h"



// Contains a command buffer and information about it
struct VulkanCommand
{
	VkCommandBuffer command_buffer;
	VkCommandPool command_pool;
	VkDevice device;
	VkQueue graphics_queue;
};

struct VulkanCommandParameters
{
	VulkanDevice device;
	VulkanCommandPool command_pool;
};

struct VulkanRenderPassCommandBufferAllocateParameters
{
	VulkanSwapChain swap_chain;
};

struct VulkanSubpassCommandBufferRecordDetails
{
	std::vector<VulkanPipeline> pipelines;

	// Keep these in the same order as the pipelines they corrospond to
	std::vector<std::vector<VulkanResource>> resources;
	std::vector<std::vector<VulkanBuffer>> vertex_buffers;
	std::vector<std::vector<VulkanBuffer>> index_buffers;
};

struct VulkanRenderPassCommandBufferRecordParameters
{
	std::vector<VulkanSubpassCommandBufferRecordDetails> subpasses;

	VulkanSwapChain swap_chain;
	VulkanDevice device;
	std::vector<VkClearValue> clear_values;
	uint32_t framebuffer_index;
};

// Initializes values in VulkanCommand
void create_command(VulkanCommand &command, VulkanCommandParameters &parameters);

// Frees memory in VulkanCommandPool
void cleanup_command(VulkanCommand &command);

// Initializes VulkanCommand for single time use
void start_single_time_command(VulkanCommand &command, VulkanCommandParameters &parameters);

// Ends single time command
void end_single_time_command(VulkanCommand &command);

// Creates command buffers for the render pass
void allocate_render_pass_command_buffers(VulkanRenderPass &render_pass, VulkanRenderPassCommandBufferAllocateParameters &parameters);

// Records command buffers for the render pass
void record_render_pass_command_buffers(VulkanRenderPass &render_pass, VulkanRenderPassCommandBufferRecordParameters &parameters);

// De-allocates command buffers for render pass
void cleanup_render_pass_command_buffers(VulkanRenderPass &render_pass);

