#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


#include <VulkanLayer/Device.h>
#include <VulkanLayer/SwapChain.h>
#include <VulkanLayer/RenderPass.h>
#include <VulkanLayer/Shader.h>
#include <VulkanLayer/Buffer.h>
#include <VulkanLayer/Texture.h>
#include <VulkanLayer/Pipeline.h>
#include <VulkanLayer/MemoryManager.h>
#include <VulkanLayer/Resource.h>

#include <unordered_map>

enum MaterialIds
{
	MATERIAL_GREEN_CUBE = 0,
	MATERIAL_RED_SQUARE = 1,
	MATERIAL_BLUE_CUBE = 2,
	MATERIAL_YELLOW_CUBE = 3,
	MATERIAL_TEXT = 4
};

struct Instance
{
	VulkanResource resource;
	uint32_t material;
};

struct Material
{
	std::string pipeline;
	std::pair<VulkanBuffer, VulkanBuffer> *model;

	// Not a pointer because this needs to be passes as-is
	std::vector<std::vector<VulkanTexture>> textures;

	std::vector<VulkanResource> *resources;
	std::vector<VulkanBuffer> *vertex_buffers;
	std::vector<VulkanBuffer> *index_buffers;
};

// TODO: Is this a good name?
struct RenderPassManager
{
	VulkanRenderPass pass;
	std::unordered_map<std::string, VulkanPipeline> pass_pipelines;
	//std::unordered_map<std::string, Instance> resources;

	std::unordered_map<std::string, std::vector<VulkanResource>> resources;
	std::unordered_map<std::string, std::vector<VulkanBuffer>> vertex_buffers;
	std::unordered_map<std::string, std::vector<VulkanBuffer>> index_buffers;
};

struct UniformBuffer
{
	std::string name;
	std::vector<VulkanBuffer> buffers;
};

struct DataManager
{
	std::unordered_map<std::string, VulkanTexture> textures;
	std::unordered_map<std::string, UniformBuffer> uniform_buffers;
	std::unordered_map<std::string, std::pair<VulkanBuffer, VulkanBuffer>> models;
	std::unordered_map<std::string, VulkanShader> shaders;
	std::vector<Material> materials;
};

struct Renderer
{
	GLFWwindow *window;
	VulkanDevice device;
	VulkanMemoryManager memory_manager;
	VulkanSwapChain swap_chain;
	std::vector<RenderPassManager> render_passes;
	DataManager data;
	uint32_t max_frames;
	uint32_t image_index;

	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> in_flight_fences;
	std::vector<VkFence> images_in_flight;

	//std::vector<std::pair<std::string, std::string>> submitted_instances;
	std::unordered_map<std::string, Instance> instances;
};

struct RendererParameters
{
	GLFWwindow *window;
	std::vector<std::string> shader_files;
	std::vector<std::string> texture_files;
	std::vector<std::string> model_files;

	std::vector<const char *> instance_extensions;
	std::vector<const char *> physical_extensions;
	uint32_t max_frames;
};

struct DrawParameters
{
	uint32_t draw_frame;
};

struct UniformBufferParameters
{
	uint32_t size;
};

struct UniformBufferUpdateParameters
{
	std::string buffer_name;
	void *data;
};

struct InstanceParameters
{
	std::string uniform_buffer;
	uint32_t material;
};
struct InstanceSubmitParameters
{
	std::string instance_name;
};

struct DataManagerParameters
{
	std::unordered_map<std::string, VulkanTexture> textures;
	std::unordered_map<std::string, UniformBuffer> uniform_buffers;
	std::unordered_map<std::string, std::pair<VulkanBuffer, VulkanBuffer>> models;
	std::unordered_map<std::string, VulkanShader> shaders;
	std::vector<Material> materials;
};

struct RenderPassManagerParameters
{
	VulkanRenderPass pass;
	std::unordered_map<std::string, VulkanPipeline> pass_pipelines;
};

// Sets up the renderer
void create_renderer(Renderer &renderer, RendererParameters &parameters);

// Sets renderer.image_index to the next value
void update_image_index(Renderer &renderer, uint32_t draw_frame);

// Draws all submitted instances
void draw(Renderer &renderer, DrawParameters &parameters);

// Cleans up the renderer
void cleanup_renderer(Renderer &renderer);

// Creates a uniform buffer
std::string get_uniform_buffer(Renderer &renderer, UniformBufferParameters &parameters);

// Copies data into uniform buffer
void update_uniform_buffer(Renderer &renderer, UniformBufferUpdateParameters &parameters);

// Cleans up a uniform buffer
void free_uniform_buffer(Renderer &renderer, std::string buffer);

// Creates an instance for use with a specific pipeline
std::string create_instance(Renderer &renderer, InstanceParameters &parameters);

// Submits an instance for rendering
void submit_instance(Renderer &renderer, InstanceSubmitParameters &parameters);

// Frees an instance
void free_instance(Renderer &renderer, std::string instance_name);

// Sets up the data manager
void create_data_manager(DataManager &data_manager, DataManagerParameters &data_manager_parameters);

// Cleans up the data manager
void cleanup_data_manager(Renderer &renderer, DataManager &data_manager);

// Sets up the render pass manager
void create_render_pass_manager(RenderPassManager &render_pass_manager, RenderPassManagerParameters &render_pass_manager_parameters);

// Cleans up the render pass manager
void cleanup_render_pass_manager(Renderer &renderer, RenderPassManager &render_pass_manager);

//  Recreates the necessary components to resize the swap chain
void resize_swap_chain(Renderer &renderer);