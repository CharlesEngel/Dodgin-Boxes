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
#include <VulkanLayer/Command.h>

#include <unordered_map>

const uint8_t max_lights = 14;

enum RenderPassIds
{
	RENDER_PASS_INDEX_SHADOW = 0,
	RENDER_PASS_INDEX_REFLECT = 1,
	RENDER_PASS_INDEX_BOX_INTERNALS = 2,
	RENDER_PASS_INDEX_DRAW = 3
};

enum MaterialIds
{
	MATERIAL_GREEN_CUBE = 0,
	MATERIAL_RED_SQUARE = 1,
	MATERIAL_BLUE_CUBE = 2,
	MATERIAL_YELLOW_CUBE = 3,
	MATERIAL_TEXT = 4
};

enum LightType
{
	LIGHT_POINT = 0
};

enum LightUsage
{
	LIGHT_USAGE_NONE = 0,
	LIGHT_USAGE_ALL = 1
};

struct Instance
{
	std::vector<VulkanResource> resources;
	uint32_t material;
};

struct Material
{
	std::vector<std::string> pipelines;
	std::vector<std::pair<VulkanBuffer, VulkanBuffer>*> models;

	// Not a pointer because this needs to be passes as-is
	std::vector<std::vector<VulkanTexture>> textures;
	LightUsage use_lights;

	std::vector<std::vector<VulkanResource>*> resources;
	std::vector<std::vector<VulkanBuffer>*> vertex_buffers;
	std::vector<std::vector<VulkanBuffer>*> index_buffers;
};

struct Light
{
	glm::vec3 location;
	glm::vec3 color;
	float intensity;
	float max_distance;
	LightType type;
	int active;
};

struct AlignedVec3
{
	alignas(16) glm::vec3 value;
};

struct AlignedFloat
{
	alignas(16) float value;
};

struct AlignedInt
{
	alignas(16) int value;
};

struct LightUniformBuffer
{
	alignas(16) AlignedVec3 location[max_lights];
	alignas(16) AlignedVec3 color[max_lights];
	alignas(16) AlignedFloat intensity[max_lights];
	alignas(16) AlignedFloat max_distance[max_lights];
	alignas(16) AlignedInt active[max_lights];
};

struct ShadowMapUniformBuffer
{
	glm::mat4 view[6];
	glm::mat4 proj[6];
	int light_index;
};

struct ReflectionMapUniformBuffer
{
	glm::mat4 view[6];
	glm::mat4 proj;
};

struct ShadowMapFragmentUniformBuffer
{
	glm::vec3 location;
};

struct BoxInternalsUniformBuffer
{
	glm::mat4 model[6];
	glm::mat4 view[6];
	glm::mat4 proj;
	int light_index;
};

// TODO: Is this a good name?
// ANSWER: Not really
struct RenderPassManager
{
	VulkanRenderPass pass;
	std::unordered_map<std::string, VulkanPipeline> pass_pipelines;

	std::unordered_map<std::string, std::vector<VulkanResource>> resources;
	std::unordered_map<std::string, std::vector<VulkanBuffer>> vertex_buffers;
	std::unordered_map<std::string, std::vector<VulkanBuffer>> index_buffers;
	std::vector<VkClearValue> clear_values;

	bool mip_map;
	VulkanMipmapGenerationParameters mip_parameters;
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

	std::unordered_map<std::string, Instance> instances;

	std::vector<Light> lights;
	std::string light_buffers;
	std::array<std::string, max_lights> shadow_map_buffers;
	std::array<std::string, max_lights> shadow_map_fragment_buffers;
	ShadowMapUniformBuffer shadow_map_uniform;
	std::string reflection_map_buffer;
	std::string box_internals_buffer;
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
	uint32_t range;
};

struct UniformBufferUpdateParameters
{
	std::string buffer_name;
	void *data;
};

struct InstanceParameters
{
	std::vector<std::vector<std::string>> uniform_buffers;
	uint32_t material;
	int light_index;
};

struct InstanceSubmitParameters
{
	std::string instance_name;
};

struct LightParameters
{
	glm::vec3 location;
	glm::vec3 color;
	float intensity;
	float max_distance;
	LightType type;
};

struct LightUpdateParameters
{
	glm::vec3 location;
	glm::vec3 color;
	float intensity;
	float max_distance;
	uint8_t light_index;
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
	std::vector<VkClearValue> clear_values;

	bool mip_map;
	VulkanMipmapGenerationParameters mip_parameters;
};

// Sets up the renderer
void create_renderer(Renderer &renderer, RendererParameters &parameters);

// Draws all submitted instances
void draw(Renderer &renderer, DrawParameters &parameters);

// Updates the uniform buffer for the reflection map
void update_reflection_map(Renderer &renderer, glm::vec3 location);

// Sets renderer.image_index to the next value
void update_image_index(Renderer &renderer, uint32_t draw_frame);

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

// Creates a light
uint8_t create_light(Renderer &renderer, LightParameters &parameters);

// Updates a light
void update_light(Renderer &renderer, LightUpdateParameters &parameters);

// Frees light
void free_light(Renderer &renderer, uint8_t light_index);

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