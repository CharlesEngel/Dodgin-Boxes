#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

#include <vector>

#include "CommandPool.h"


// Includes up to the device, allowing a common framework for various pipelines to be built on
struct VulkanDevice
{
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice device;
	VkQueue graphics_queue;
	VkQueue present_queue;
	VkSurfaceKHR surface;
	uint32_t graphics_family_index;
	uint32_t present_family_index;
	VulkanCommandPool command_pool;
	VkSampleCountFlagBits max_sample_count;
};

// Settings and requirements for how components are made/which physical device is chosen.
struct VulkanDeviceParameters
{
	std::vector<const char *> required_instance_extensions;
	std::vector<const char *> required_device_extensions;
	std::vector<const char *> validation_layers;
	GLFWwindow *glfw_window;
	bool validation_layers_enabled;
};


// Callback for validation layers
VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data);

// Fills the Device structure by creating all the vulkan objects.
void create_device(VulkanDevice &device, const VulkanDeviceParameters &requirements);

// Cleans up Device structure
void cleanup_device(VulkanDevice &device);