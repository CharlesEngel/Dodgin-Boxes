#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <vector>

#include "Device.h"

// Contains a swap chain and its images and framebuffers
struct VulkanSwapChain
{
	VkSwapchainKHR swap_chain;
	std::vector<VkImage> swap_chain_images;
	VkFormat swap_chain_format;
	VkExtent2D swap_chain_extent;
	std::vector<VkImageView> swap_chain_image_views;
	VkDevice device;
};

struct VulkanSwapChainParameters
{
	VulkanDevice device;
	GLFWwindow *glfw_window;

};

// Initializes values in VulkanSwapChain
void create_swap_chain(VulkanSwapChain &swap_chain, VulkanSwapChainParameters &parameters);

// Frees memory in VulkanSwapChain
void cleanup_swap_chain(VulkanSwapChain &swap_chain);