#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <vector>

#include "Device.h"
#include "SwapChain.h"
#include "MemoryManager.h"
#include "Texture.h"

// Contains render pass and its image views and command pool
struct VulkanRenderPass
{
	VkDevice device;
	VulkanMemoryManager *memory_manager;
	VkCommandPool command_pool;
	VkRenderPass render_pass;
	std::vector<VkFramebuffer> framebuffers;

	uint32_t total_subpasses;

	VkImageView color_image_view;
	VkImageView depth_image_view;
	VulkanMemory color_image_memory;
	VulkanMemory depth_image_memory;
	VkImage color_image;
	VkImage depth_image;
	VkExtent2D draw_extent;

	std::vector<VkCommandBuffer> command_buffers;
};

// TODO: Looks like VulkanRenderPassAttachment could be combined with this pretty easily.
// Contains a final attachment to draw to (such as a set of textures) 
// NOTE: This has a different attachment for each swap chain image
struct VulkanRenderPassFramebufferAttachment
{
	std::vector<VkImageView> image_views;
	uint32_t width, height;
	VkImageLayout initial_layout, final_layout;
	VkFormat format;
};

// Constains information for a single attachment.
struct VulkanRenderPassAttachment
{
	VulkanTexture attachment;
	VkImageLayout initial_layout;
	VkImageLayout final_layout;
	VkSampleCountFlagBits samples;
	VkFormat attachment_format;
};

// Describes a single subpass
struct VulkanRenderPassSubpassDescription
{
	std::vector<uint32_t> color_attachments;
	std::vector<uint32_t> input_attachments;
	std::vector<uint32_t> resolve_attachments;
	uint32_t depth_attachment;
	bool use_depth;

	std::vector<VkSubpassDependency> dependencies;
};

// Combines attachments and subpass descriptions
struct VulkanRenderPassSubpasses
{
	std::vector<VulkanRenderPassAttachment> attachments;
	std::vector<VulkanRenderPassSubpassDescription> subpass_descriptions;
};

struct VulkanRenderPassParameters
{
	VulkanDevice device;
	VulkanSwapChain swap_chain;
	VulkanMemoryManager *memory_manager;
	GLFWwindow *glfw_window;

	VulkanRenderPassFramebufferAttachment attachments;
	VulkanRenderPassSubpasses subpasses;

};

// Initializes values in VulkanRenderPass
void create_render_pass(VulkanRenderPass &render_pass, VulkanRenderPassParameters &parameters);

// Get format for depth attachment
VkFormat find_depth_format(VkPhysicalDevice physical_device);

// Frees memory in VulkanRenderPass
void cleanup_render_pass(VulkanRenderPass &render_pass);