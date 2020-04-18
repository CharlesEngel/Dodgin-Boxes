#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <vector>

#include "Device.h"
#include "SwapChain.h"

// Contains render pass and its image views and command pool
struct VulkanRenderPass
{
	VkDevice device;
	VkCommandPool command_pool;
	VkRenderPass render_pass;
	// TODO: Move into Swap Chain? (Maybe give render passes their own optional framebuffers?)
	std::vector<VkFramebuffer> framebuffers;

	VkImageView color_image_view;
	VkImageView depth_image_view;
	VkDeviceMemory color_image_memory;
	VkDeviceMemory depth_image_memory;
	VkImage color_image;
	VkImage depth_image;
	VkExtent2D draw_extent;

	std::vector<VkCommandBuffer> command_buffers;
};

struct VulkanRenderPassFramebufferAttachment
{
	std::vector<VkImageView> image_views;
	uint32_t width, height;
	VkImageLayout initial_layout, final_layout;
	VkFormat format;
};

struct VulkanRenderPassParameters
{
	VulkanDevice device;
	VulkanSwapChain swap_chain;
	GLFWwindow *glfw_window;

	VulkanRenderPassFramebufferAttachment attachments;

};

// Initializes values in VulkanRenderPass
void create_render_pass(VulkanRenderPass &render_pass, VulkanRenderPassParameters &parameters);

// Frees memory in VulkanRenderPass
void cleanup_render_pass(VulkanRenderPass &render_pass);