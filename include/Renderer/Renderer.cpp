#include "Renderer.h"

#include <Command.h>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <algorithm>
#include "Utilities.h"

const bool enable_validation_layers = true;

void create_renderer(Renderer &renderer, RendererParameters &parameters)
{
	renderer.instances = {};
	renderer.max_frames = parameters.max_frames;

	// Copy window and enable validation layers
	if (enable_validation_layers) {
		parameters.instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	renderer.window = parameters.window;

	// Create device
	VulkanDeviceParameters device_parameters = {};
	device_parameters.glfw_window = renderer.window;
	device_parameters.required_instance_extensions = parameters.instance_extensions;
	device_parameters.required_device_extensions = parameters.physical_extensions;
	device_parameters.validation_layers_enabled = enable_validation_layers;
	device_parameters.validation_layers = { "VK_LAYER_KHRONOS_validation" };

	create_device(renderer.device, device_parameters);

	// Create memory manager
	VulkanMemoryManagerParameters memory_manager_parameters = {};
	memory_manager_parameters.device = renderer.device;

	create_memory_manager(renderer.memory_manager, memory_manager_parameters);

	// Create swap chain
	VulkanSwapChainParameters swap_chain_parameters = {};
	swap_chain_parameters.device = renderer.device;
	swap_chain_parameters.glfw_window = renderer.window;

	create_swap_chain(renderer.swap_chain, swap_chain_parameters);

	// Create shaders
	std::unordered_map <std::string, VulkanShader> shaders = {};
	for (auto shader_file : parameters.shader_files)
	{
		VulkanShader shader = {};
		VulkanShaderParameters shader_parameters = {};
		shader_parameters.device = renderer.device;
		shader_parameters.file = shader_file;
		if (shader_file.substr(10, 4) == "vert")
		{
			shader_parameters.type = 'v';
		}
		else if (shader_file.substr(10, 4) == "frag")
		{
			shader_parameters.type = 'f';
		}

		create_shader(shader, shader_parameters);
		shaders[shader_file] = shader;
	}

	// Create textures
	std::unordered_map<std::string, VulkanTexture> textures = {};
	for (auto texture_file : parameters.texture_files)
	{
		VulkanTextureData texture_data = {};
		VulkanTextureDataParameters texture_data_parameters = {};
		texture_data_parameters.filename = texture_file;
		load_image(texture_data, texture_data_parameters);

		VulkanTexture texture = {};
		VulkanTextureParameters texture_parameters = {};
		texture_parameters.command_pool = renderer.device.command_pool;
		texture_parameters.data = texture_data;
		texture_parameters.device = renderer.device;
		texture_parameters.memory_manager = &(renderer.memory_manager);
		texture_parameters.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		create_texture(texture, texture_parameters);
		cleanup_image(texture_data);
		textures[texture_file] = texture;
	}

	// Create textures for framebuffer attachments
	VulkanTexture color_attachment = {};
	VulkanTextureParameters color_attachment_parameters = {};
	color_attachment_parameters.device = renderer.device;
	color_attachment_parameters.command_pool = renderer.device.command_pool;
	color_attachment_parameters.memory_manager = &renderer.memory_manager;
	color_attachment_parameters.format = renderer.swap_chain.swap_chain_format;
	color_attachment_parameters.height = renderer.swap_chain.swap_chain_extent.height;
	color_attachment_parameters.width = renderer.swap_chain.swap_chain_extent.width;
	color_attachment_parameters.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	color_attachment_parameters.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	create_texture(color_attachment, color_attachment_parameters);

	textures["RENDER_PASS_ATTACHMENT_COLOR"] = color_attachment;

	VulkanTexture depth_attachment = {};
	VulkanTextureParameters depth_attachment_parameters = {};
	depth_attachment_parameters.device = renderer.device;
	depth_attachment_parameters.command_pool = renderer.device.command_pool;
	depth_attachment_parameters.memory_manager = &renderer.memory_manager;
	depth_attachment_parameters.format = find_depth_format(renderer.device.physical_device);
	depth_attachment_parameters.height = renderer.swap_chain.swap_chain_extent.height;
	depth_attachment_parameters.width = renderer.swap_chain.swap_chain_extent.width;
	depth_attachment_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	depth_attachment_parameters.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	create_texture(depth_attachment, depth_attachment_parameters);

	textures["RENDER_PASS_ATTACHMENT_DEPTH"] = depth_attachment;

	VulkanTexture volume_depth_attachment = {};
	VulkanTextureParameters volume_depth_attachment_parameters = {};
	volume_depth_attachment_parameters.device = renderer.device;
	volume_depth_attachment_parameters.command_pool = renderer.device.command_pool;
	volume_depth_attachment_parameters.memory_manager = &renderer.memory_manager;
	volume_depth_attachment_parameters.format = find_depth_format(renderer.device.physical_device);
	volume_depth_attachment_parameters.height = renderer.swap_chain.swap_chain_extent.height;
	volume_depth_attachment_parameters.width = renderer.swap_chain.swap_chain_extent.width;
	volume_depth_attachment_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	volume_depth_attachment_parameters.samples = VK_SAMPLE_COUNT_1_BIT;

	create_texture(volume_depth_attachment, volume_depth_attachment_parameters);

	textures["RENDER_PASS_ATTACHMENT_VOLUME_DEPTH"] = volume_depth_attachment;

	// Create attachment for shadows
	VulkanTexture shadow_map_attachment_0 = {};
	VulkanTexture shadow_map_attachment_1 = {};
	VulkanTexture shadow_map_attachment_2 = {};
	VulkanTexture shadow_map_attachment_3 = {};
	VulkanTexture shadow_map_attachment_4 = {};
	VulkanTexture shadow_map_attachment_5 = {};
	VulkanTexture shadow_map_attachment_6 = {};
	VulkanTexture shadow_map_attachment_7 = {};
	VulkanTexture shadow_map_attachment_8 = {};
	VulkanTexture shadow_map_attachment_9 = {};
	VulkanTexture shadow_map_attachment_10 = {};
	VulkanTexture shadow_map_attachment_11 = {};
	VulkanTexture shadow_map_attachment_12 = {};
	VulkanTexture shadow_map_attachment_13 = {};
	VulkanTextureParameters shadow_map_attachment_parameters = {};
	shadow_map_attachment_parameters.device = renderer.device;
	shadow_map_attachment_parameters.command_pool = renderer.device.command_pool;
	shadow_map_attachment_parameters.memory_manager = &renderer.memory_manager;
	shadow_map_attachment_parameters.format = find_depth_format(renderer.device.physical_device);
	shadow_map_attachment_parameters.width = 128;
	shadow_map_attachment_parameters.height = 128;
	shadow_map_attachment_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	shadow_map_attachment_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	shadow_map_attachment_parameters.layers = 6;
	shadow_map_attachment_parameters.flags = TextureFlags(TEXTURE_CUBE | TEXTURE_SAMPLER_COMPARE_LESS_OR_EQUAL | TEXTURE_BORDER_WHITE);

	create_texture(shadow_map_attachment_0, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_1, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_2, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_3, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_4, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_5, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_6, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_7, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_8, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_9, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_10, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_11, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_12, shadow_map_attachment_parameters);
	create_texture(shadow_map_attachment_13, shadow_map_attachment_parameters);

	textures["SHADOW_MAP_ATTACHMENT_0"] = shadow_map_attachment_0;
	textures["SHADOW_MAP_ATTACHMENT_1"] = shadow_map_attachment_1;
	textures["SHADOW_MAP_ATTACHMENT_2"] = shadow_map_attachment_2;
	textures["SHADOW_MAP_ATTACHMENT_3"] = shadow_map_attachment_3;
	textures["SHADOW_MAP_ATTACHMENT_4"] = shadow_map_attachment_4;
	textures["SHADOW_MAP_ATTACHMENT_5"] = shadow_map_attachment_5;
	textures["SHADOW_MAP_ATTACHMENT_6"] = shadow_map_attachment_6;
	textures["SHADOW_MAP_ATTACHMENT_7"] = shadow_map_attachment_7;
	textures["SHADOW_MAP_ATTACHMENT_8"] = shadow_map_attachment_8;
	textures["SHADOW_MAP_ATTACHMENT_9"] = shadow_map_attachment_9;
	textures["SHADOW_MAP_ATTACHMENT_10"] = shadow_map_attachment_10;
	textures["SHADOW_MAP_ATTACHMENT_11"] = shadow_map_attachment_11;
	textures["SHADOW_MAP_ATTACHMENT_12"] = shadow_map_attachment_12;
	textures["SHADOW_MAP_ATTACHMENT_13"] = shadow_map_attachment_13;

	// Create attachment for reflection map
	VulkanTexture reflection_map_attachment = {};
	VulkanTextureParameters reflection_map_attachment_parameters = {};
	reflection_map_attachment_parameters.device = renderer.device;
	reflection_map_attachment_parameters.command_pool = renderer.device.command_pool;
	reflection_map_attachment_parameters.memory_manager = &renderer.memory_manager;
	reflection_map_attachment_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	reflection_map_attachment_parameters.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	reflection_map_attachment_parameters.width = 128;
	reflection_map_attachment_parameters.height = 128;
	reflection_map_attachment_parameters.layers = 6;
	reflection_map_attachment_parameters.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	reflection_map_attachment_parameters.flags = TEXTURE_CUBE;

	create_texture(reflection_map_attachment, reflection_map_attachment_parameters);
	textures["REFLECTION_MAP_ATTACHMENT"] = reflection_map_attachment;

	VulkanTexture reflection_map_final = {};
	VulkanTextureParameters reflection_map_final_parameters = {};
	reflection_map_final_parameters.device = renderer.device;
	reflection_map_final_parameters.command_pool = renderer.device.command_pool;
	reflection_map_final_parameters.memory_manager = &renderer.memory_manager;
	reflection_map_final_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	reflection_map_final_parameters.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	reflection_map_final_parameters.width = 128;
	reflection_map_final_parameters.height = 128;
	reflection_map_final_parameters.layers = 6;
	reflection_map_final_parameters.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	reflection_map_final_parameters.flags = TEXTURE_CUBE;
	reflection_map_final_parameters.mip_levels = 8;

	create_texture(reflection_map_final, reflection_map_final_parameters);
	textures["REFLECTION_MAP_FINAL"] = reflection_map_final;

	VulkanTexture reflection_map_depth_attachment = {};
	VulkanTextureParameters reflection_map_depth_attachment_parameters = {};
	reflection_map_depth_attachment_parameters.device = renderer.device;
	reflection_map_depth_attachment_parameters.command_pool = renderer.device.command_pool;
	reflection_map_depth_attachment_parameters.memory_manager = &renderer.memory_manager;
	reflection_map_depth_attachment_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	reflection_map_depth_attachment_parameters.format = find_depth_format(renderer.device.physical_device);
	reflection_map_depth_attachment_parameters.width = 128;
	reflection_map_depth_attachment_parameters.height = 128;
	reflection_map_depth_attachment_parameters.layers = 6;
	reflection_map_depth_attachment_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	reflection_map_depth_attachment_parameters.flags = TEXTURE_CUBE;

	create_texture(reflection_map_depth_attachment, reflection_map_depth_attachment_parameters);
	textures["REFLECTION_MAP_DEPTH_ATTACHMENT"] = reflection_map_depth_attachment;

	// Create texture for internals of glass box
	VulkanTexture box_internals_attachment = {};
	VulkanTextureParameters box_internals_attachment_parameters = {};
	box_internals_attachment_parameters.device = renderer.device;
	box_internals_attachment_parameters.command_pool = renderer.device.command_pool;
	box_internals_attachment_parameters.memory_manager = &renderer.memory_manager;
	box_internals_attachment_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	box_internals_attachment_parameters.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	box_internals_attachment_parameters.width = 64;
	box_internals_attachment_parameters.height = 64;
	box_internals_attachment_parameters.layers = 6;
	box_internals_attachment_parameters.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	box_internals_attachment_parameters.flags = TEXTURE_CUBE;

	create_texture(box_internals_attachment, box_internals_attachment_parameters);
	textures["BOX_INTERNALS_ATTACHMENT"] = box_internals_attachment;

	VulkanTexture box_internals_final = {};
	VulkanTextureParameters box_internals_final_parameters = {};
	box_internals_final_parameters.device = renderer.device;
	box_internals_final_parameters.command_pool = renderer.device.command_pool;
	box_internals_final_parameters.memory_manager = &renderer.memory_manager;
	box_internals_final_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	box_internals_final_parameters.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	box_internals_final_parameters.width = 64;
	box_internals_final_parameters.height = 64;
	box_internals_final_parameters.layers = 6;
	box_internals_final_parameters.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	box_internals_final_parameters.flags = TEXTURE_CUBE;
	box_internals_final_parameters.mip_levels = 7;

	create_texture(box_internals_final, box_internals_final_parameters);
	textures["BOX_INTERNALS_FINAL"] = box_internals_final;

	VulkanTexture box_internals_depth_attachment = {};
	VulkanTextureParameters box_internals_attachment_depth_parameters = {};
	box_internals_attachment_depth_parameters.device = renderer.device;
	box_internals_attachment_depth_parameters.command_pool = renderer.device.command_pool;
	box_internals_attachment_depth_parameters.memory_manager = &renderer.memory_manager;
	box_internals_attachment_depth_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	box_internals_attachment_depth_parameters.format = find_depth_format(renderer.device.physical_device);
	box_internals_attachment_depth_parameters.width = 64;
	box_internals_attachment_depth_parameters.height = 64;
	box_internals_attachment_depth_parameters.layers = 6;
	box_internals_attachment_depth_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	box_internals_attachment_depth_parameters.flags = TEXTURE_CUBE;

	create_texture(box_internals_depth_attachment, box_internals_attachment_depth_parameters);
	textures["BOX_INTERNALS_DEPTH_ATTACHMENT"] = box_internals_depth_attachment;

	// Create models and binding/attribute descriptions
	struct VertexWithTexCoord
	{
		alignas(16) glm::vec3 point;
		alignas(8) glm::vec2 tex_coord;
	};

	struct VertexWithNormal
	{
		alignas(16) glm::vec3 point;
		alignas(16) glm::vec3 normal;
	};

	std::unordered_map<std::string, std::pair<VulkanBuffer, VulkanBuffer>> models = {};
	VkVertexInputBindingDescription binding_description = {};
	binding_description.binding = 0;
	binding_description.stride = sizeof(VertexWithNormal);
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputBindingDescription> binding_descriptions = { binding_description };

	VkVertexInputBindingDescription binding_description_tex_coord = {};
	binding_description_tex_coord.binding = 0;
	binding_description_tex_coord.stride = sizeof(VertexWithTexCoord);
	binding_description_tex_coord.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputBindingDescription> binding_descriptions_tex_coords = { binding_description_tex_coord };

	VkVertexInputAttributeDescription attribute_description = {};

	attribute_description.binding = 0;
	attribute_description.location = 0;
	attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description.offset = offsetof(VertexWithNormal, point);

	VkVertexInputAttributeDescription attribute_description_normal = {};

	attribute_description_normal.binding = 0;
	attribute_description_normal.location = 1;
	attribute_description_normal.format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description_normal.offset = offsetof(VertexWithNormal, normal);

	VkVertexInputAttributeDescription attribute_description_tex_coord_location = {};

	attribute_description_tex_coord_location.binding = 0;
	attribute_description_tex_coord_location.location = 0;
	attribute_description_tex_coord_location.format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description_tex_coord_location.offset = offsetof(VertexWithTexCoord, point);

	VkVertexInputAttributeDescription attribute_description_tex_coord_coord = {};

	attribute_description_tex_coord_coord.binding = 0;
	attribute_description_tex_coord_coord.location = 1;
	attribute_description_tex_coord_coord.format = VK_FORMAT_R32G32_SFLOAT;
	attribute_description_tex_coord_coord.offset = offsetof(VertexWithTexCoord, tex_coord);
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions_tex_coords = { attribute_description_tex_coord_location, attribute_description_tex_coord_coord };
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions = { attribute_description, attribute_description_normal };

	std::vector<VertexWithNormal> square_vertices_data = {
		{{-0.5f, -0.5f, 0.f}, {0.0f, 0.0f, 1.0f}},
		{{0.5f, -0.5f, 0.f}, {0.0f, 0.0f, 1.0f}},
		{{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}}
	};

	std::vector<VertexWithTexCoord> square_vertices_tex_coords_data = {
		{{-0.5f, -0.5f, 0.5f}, {0.0, 1.0}},
		{{0.5f, -0.5f, 0.5f}, {1.0, 1.0}},
		{{0.5f, 0.5f, 0.5f}, {1.0, 0.0}},
		{{-0.5f, 0.5f, 0.5f}, {0.0, 0.0}}
	};

	std::vector<uint32_t> square_indices_data = {
		0, 1, 2, 2, 3, 0
	};

	VulkanBufferParameters square_vertices_parameters = {};
	square_vertices_parameters.data = (void *)square_vertices_data.data();
	square_vertices_parameters.device = renderer.device;
	square_vertices_parameters.memory_manager = &(renderer.memory_manager);
	square_vertices_parameters.range = sizeof(VertexWithNormal);
	square_vertices_parameters.size = sizeof(VertexWithNormal) * square_vertices_data.size();
	square_vertices_parameters.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	square_vertices_parameters.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VulkanBuffer square_vertices_buffer = {};
	create_buffer(square_vertices_buffer, square_vertices_parameters);

	VulkanBufferParameters square_vertices_tex_coords_parameters = {};
	square_vertices_tex_coords_parameters.data = (void *)square_vertices_tex_coords_data.data();
	square_vertices_tex_coords_parameters.device = renderer.device;
	square_vertices_tex_coords_parameters.memory_manager = &(renderer.memory_manager);
	square_vertices_tex_coords_parameters.range = sizeof(VertexWithTexCoord);
	square_vertices_tex_coords_parameters.size = sizeof(VertexWithTexCoord) * square_vertices_tex_coords_data.size();
	square_vertices_tex_coords_parameters.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	square_vertices_tex_coords_parameters.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VulkanBuffer square_vertices_tex_coords_buffer = {};
	create_buffer(square_vertices_tex_coords_buffer, square_vertices_tex_coords_parameters);

	VulkanBufferParameters square_indices_parameters = {};
	square_indices_parameters.data = (void *)square_indices_data.data();
	square_indices_parameters.device = renderer.device;
	square_indices_parameters.memory_manager = &(renderer.memory_manager);
	square_indices_parameters.range = sizeof(uint32_t);
	square_indices_parameters.size = sizeof(uint32_t) * square_indices_data.size();
	square_indices_parameters.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	square_indices_parameters.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VulkanBuffer square_indices_buffer = {};
	create_buffer(square_indices_buffer, square_indices_parameters);

	models["SQUARE"] = { square_vertices_buffer, square_indices_buffer };
	models["SQUARE_TEX_COORDS"] = { square_vertices_tex_coords_buffer, square_indices_buffer };

	std::vector<VertexWithNormal> cube_vertices_data = {
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}},

		{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},

		{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
		{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},
		{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}},

		{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}
	};

	std::vector<uint32_t> cube_indices_data = {
		0, 2, 1, 3, 2, 0,
		4, 6, 5, 5, 6, 7,
		8, 9, 10, 10, 9, 11,
		14, 15, 13, 13, 15, 12,
		19, 18, 17, 17, 18, 16,
		20, 21, 22, 22, 21, 23
	};

	VulkanBufferParameters cube_vertices_parameters = {};
	cube_vertices_parameters.data = (void *)cube_vertices_data.data();
	cube_vertices_parameters.device = renderer.device;
	cube_vertices_parameters.memory_manager = &(renderer.memory_manager);
	cube_vertices_parameters.range = sizeof(VertexWithNormal);
	cube_vertices_parameters.size = sizeof(VertexWithNormal) * cube_vertices_data.size();
	cube_vertices_parameters.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	cube_vertices_parameters.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VulkanBuffer cube_vertices_buffer = {};
	create_buffer(cube_vertices_buffer, cube_vertices_parameters);

	VulkanBufferParameters cube_indices_parameters = {};
	cube_indices_parameters.data = (void *)cube_indices_data.data();
	cube_indices_parameters.device = renderer.device;
	cube_indices_parameters.memory_manager = &(renderer.memory_manager);
	cube_indices_parameters.range = sizeof(uint32_t);
	cube_indices_parameters.size = sizeof(uint32_t) * cube_indices_data.size();
	cube_indices_parameters.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	cube_indices_parameters.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VulkanBuffer cube_indices_buffer = {};
	create_buffer(cube_indices_buffer, cube_indices_parameters);

	models["CUBE"] = { cube_vertices_buffer, cube_indices_buffer };

	// Create data manager
	DataManagerParameters data_manager_parameters = {};
	data_manager_parameters.models = models;
	data_manager_parameters.shaders = shaders;
	data_manager_parameters.textures = textures;
	data_manager_parameters.uniform_buffers = {};
	data_manager_parameters.materials = {};

	create_data_manager(renderer.data, data_manager_parameters);

	// Create render passes
	VulkanRenderPass render_pass = {};
	VulkanRenderPassParameters render_pass_parameters = {};
	render_pass_parameters.device = renderer.device;
	render_pass_parameters.glfw_window = renderer.window;
	render_pass_parameters.memory_manager = &(renderer.memory_manager);
	render_pass_parameters.swap_chain = renderer.swap_chain;

	// Description for swap chain attachment
	VulkanRenderPassAttachment swap_chain_attachment_description = {};
	swap_chain_attachment_description.attachment_format = renderer.swap_chain.swap_chain_format;
	swap_chain_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	swap_chain_attachment_description.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	swap_chain_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	// Description for color attachment (Before downsampling)
	VulkanRenderPassAttachment color_attachment_description = {};
	color_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_COLOR"];
	color_attachment_description.attachment_format = color_attachment_description.attachment.format;
	color_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	color_attachment_description.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	// Description for depth attachment
	VulkanRenderPassAttachment depth_attachment_description = {};
	depth_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"];
	depth_attachment_description.attachment_format = depth_attachment_description.attachment.format;
	depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment_description.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	// Description for volumetric lighting depth attachment
	VulkanRenderPassAttachment volume_depth_attachment_description = {};
	volume_depth_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_VOLUME_DEPTH"];
	volume_depth_attachment_description.attachment_format = volume_depth_attachment_description.attachment.format;
	volume_depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	volume_depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	volume_depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	// Dependency between volumetric lighting subpass and text subpass
	VkSubpassDependency volume_text_dependency = {};
	volume_text_dependency.srcSubpass = 1;
	volume_text_dependency.dstSubpass = 2;
	volume_text_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	volume_text_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	volume_text_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	volume_text_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	volume_text_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Dependency between opaque subpass and volume subpass
	VkSubpassDependency opaque_volume_color_dependency = {};
	opaque_volume_color_dependency.srcSubpass = 0;
	opaque_volume_color_dependency.dstSubpass = 1;
	opaque_volume_color_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	opaque_volume_color_dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	opaque_volume_color_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	opaque_volume_color_dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	opaque_volume_color_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency opaque_volume_depth_dependency = {};
	opaque_volume_depth_dependency.srcSubpass = 0;
	opaque_volume_depth_dependency.dstSubpass = 1;
	opaque_volume_depth_dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	opaque_volume_depth_dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	opaque_volume_depth_dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	opaque_volume_depth_dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	opaque_volume_depth_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Description of opaque subpass
	VulkanRenderPassSubpassDescription subpass_opaque_description = {};
	subpass_opaque_description.color_attachments = { 1 };
	subpass_opaque_description.depth_attachment = 2;
	subpass_opaque_description.use_depth = true;
	subpass_opaque_description.dependencies = {};

	// Description of volumetric lighting subpass
	VulkanRenderPassSubpassDescription subpass_volume_description = {};
	subpass_volume_description.color_attachments = { 0 };
	subpass_volume_description.depth_attachment = 3;
	subpass_volume_description.input_attachments = { 1, 2 };
	subpass_volume_description.use_depth = true;
	subpass_volume_description.dependencies = { opaque_volume_color_dependency, opaque_volume_depth_dependency };

	// Description of text subpass
	VulkanRenderPassSubpassDescription subpass_text_description = {};
	subpass_text_description.color_attachments = { 0 };
	subpass_text_description.depth_attachment = { 3 };
	subpass_text_description.use_depth = true;
	subpass_text_description.dependencies = { volume_text_dependency };

	VulkanRenderPassSubpasses subpasses = {};
	subpasses.attachments = { swap_chain_attachment_description, color_attachment_description, depth_attachment_description, volume_depth_attachment_description };
	subpasses.subpass_descriptions = { subpass_opaque_description, subpass_volume_description, subpass_text_description };

	render_pass_parameters.subpasses = subpasses;

	create_render_pass(render_pass, render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters command_buffer_parameters = {};
	command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(render_pass, command_buffer_parameters);


	// Create shadow map subpass and attachment descriptions
	std::vector<VulkanRenderPassAttachment> shadow_map_attachment_descriptions = {};
	std::vector<VulkanRenderPassSubpassDescription> shadow_map_subpass_descriptions = {};
	for (uint32_t i = 0; i < max_lights; i++)
	{
		VulkanRenderPassAttachment shadow_map_attachment_description = {};
		shadow_map_attachment_description.attachment = renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)];
		shadow_map_attachment_description.attachment_format = shadow_map_attachment_description.attachment.format;
		shadow_map_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		shadow_map_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		shadow_map_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		shadow_map_attachment_descriptions.push_back(shadow_map_attachment_description);

		VulkanRenderPassSubpassDescription shadow_map_subpass_description = {};
		shadow_map_subpass_description.depth_attachment = i;
		shadow_map_subpass_description.use_depth = true;
		shadow_map_subpass_description.color_attachments = {};
		shadow_map_subpass_description.input_attachments = {};
		shadow_map_subpass_description.resolve_attachments = {};
		shadow_map_subpass_description.dependencies = {};
		shadow_map_subpass_descriptions.push_back(shadow_map_subpass_description);
	}

	VulkanRenderPassSubpasses shadow_map_subpasses = {};
	shadow_map_subpasses.attachments = shadow_map_attachment_descriptions;
	shadow_map_subpasses.subpass_descriptions = shadow_map_subpass_descriptions;

	// Create render pass for shadow maps
	VulkanRenderPass shadow_map_render_pass = {};
	VulkanRenderPassParameters shadow_map_render_pass_parameters = {};
	shadow_map_render_pass_parameters.device = renderer.device;
	shadow_map_render_pass_parameters.glfw_window = renderer.window;
	shadow_map_render_pass_parameters.memory_manager = &renderer.memory_manager;
	shadow_map_render_pass_parameters.swap_chain = renderer.swap_chain;
	shadow_map_render_pass_parameters.subpasses = shadow_map_subpasses;
	shadow_map_render_pass_parameters.flags = static_cast<RenderPassFlags>(RENDER_PASS_IGNORE_DRAW_IMAGES | RENDER_PASS_MULTIVIEW);
	shadow_map_render_pass_parameters.num_views = 6;
	shadow_map_render_pass_parameters.view_masks = std::vector<uint32_t>(14, 0b00111111);

	create_render_pass(shadow_map_render_pass, shadow_map_render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters shadow_map_command_buffer_parameters = {};
	shadow_map_command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(shadow_map_render_pass, shadow_map_command_buffer_parameters);

	// Create reflection map attachments and subpasses
	VulkanRenderPassAttachment reflection_map_attachment_description = {};
	reflection_map_attachment_description.attachment = renderer.data.textures["REFLECTION_MAP_ATTACHMENT"];
	reflection_map_attachment_description.attachment_format = reflection_map_attachment_description.attachment.format;
	reflection_map_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	reflection_map_attachment_description.final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	reflection_map_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassAttachment reflection_map_depth_attachment_description = {};
	reflection_map_depth_attachment_description.attachment = renderer.data.textures["REFLECTION_MAP_DEPTH_ATTACHMENT"];
	reflection_map_depth_attachment_description.attachment_format = reflection_map_depth_attachment_description.attachment.format;
	reflection_map_depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	reflection_map_depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	reflection_map_depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassSubpassDescription reflection_map_subpass_description = {};
	reflection_map_subpass_description.color_attachments = { 0 };
	reflection_map_subpass_description.depth_attachment = 1;
	reflection_map_subpass_description.use_depth = true;

	VulkanRenderPassSubpasses reflection_map_subpasses = {};
	reflection_map_subpasses.attachments = { reflection_map_attachment_description, reflection_map_depth_attachment_description };
	reflection_map_subpasses.subpass_descriptions = { reflection_map_subpass_description };

	// Create reflection map render pass
	VulkanRenderPass reflection_map_render_pass = {};
	VulkanRenderPassParameters reflection_map_render_pass_parameters = {};
	reflection_map_render_pass_parameters.device = renderer.device;
	reflection_map_render_pass_parameters.glfw_window = renderer.window;
	reflection_map_render_pass_parameters.memory_manager = &renderer.memory_manager;
	reflection_map_render_pass_parameters.swap_chain = renderer.swap_chain;
	reflection_map_render_pass_parameters.subpasses = reflection_map_subpasses;
	reflection_map_render_pass_parameters.flags = static_cast<RenderPassFlags>(RENDER_PASS_IGNORE_DRAW_IMAGES | RENDER_PASS_MULTIVIEW);
	reflection_map_render_pass_parameters.num_views = 6;
	reflection_map_render_pass_parameters.view_masks = std::vector<uint32_t>(14, 0b00111111);

	create_render_pass(reflection_map_render_pass, reflection_map_render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters reflection_map_command_buffer_parameters = {};
	reflection_map_command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(reflection_map_render_pass, reflection_map_command_buffer_parameters);

	// Create box internals attachments and subpasses
	VulkanRenderPassAttachment box_internals_attachment_description = {};
	box_internals_attachment_description.attachment = renderer.data.textures["BOX_INTERNALS_ATTACHMENT"];
	box_internals_attachment_description.attachment_format = box_internals_attachment_description.attachment.format;
	box_internals_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	box_internals_attachment_description.final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	box_internals_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassAttachment box_internals_depth_attachment_description = {};
	box_internals_depth_attachment_description.attachment = renderer.data.textures["BOX_INTERNALS_DEPTH_ATTACHMENT"];
	box_internals_depth_attachment_description.attachment_format = box_internals_depth_attachment_description.attachment.format;
	box_internals_depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	box_internals_depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	box_internals_depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassSubpassDescription box_internals_subpass_description = {};
	box_internals_subpass_description.color_attachments = { 0 };
	box_internals_subpass_description.depth_attachment = 1;
	box_internals_subpass_description.use_depth = true;

	VulkanRenderPassSubpasses box_internals_subpasses = {};
	box_internals_subpasses.attachments = { box_internals_attachment_description, box_internals_depth_attachment_description };
	box_internals_subpasses.subpass_descriptions = { box_internals_subpass_description };

	// Create box internals render pass
	VulkanRenderPass box_internals_render_pass = {};
	VulkanRenderPassParameters box_internals_render_pass_parameters = {};
	box_internals_render_pass_parameters.device = renderer.device;
	box_internals_render_pass_parameters.glfw_window = renderer.window;
	box_internals_render_pass_parameters.memory_manager = &renderer.memory_manager;
	box_internals_render_pass_parameters.swap_chain = renderer.swap_chain;
	box_internals_render_pass_parameters.subpasses = box_internals_subpasses;
	box_internals_render_pass_parameters.flags = static_cast<RenderPassFlags>(RENDER_PASS_IGNORE_DRAW_IMAGES | RENDER_PASS_MULTIVIEW);
	box_internals_render_pass_parameters.num_views = 6;
	box_internals_render_pass_parameters.view_masks = std::vector<uint32_t>(14, 0b00111111);

	create_render_pass(box_internals_render_pass, box_internals_render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters box_internals_command_buffer_parameters = {};
	box_internals_command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(box_internals_render_pass, box_internals_command_buffer_parameters);

	// Create pipeline barriers
	std::vector<VulkanPipelineBarrier> shadow_pipeline_barriers = {};

	for (uint32_t i = 0; i < max_lights; i++)
	{
		VulkanPipelineBarrier floor_shadow_pipeline_barrier = {};
		floor_shadow_pipeline_barrier.images = std::vector<VulkanTexture>(renderer.swap_chain.swap_chain_images.size(), renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)]);
		floor_shadow_pipeline_barrier.old_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		floor_shadow_pipeline_barrier.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		floor_shadow_pipeline_barrier.src = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		floor_shadow_pipeline_barrier.dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		floor_shadow_pipeline_barrier.src_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		floor_shadow_pipeline_barrier.dst_access = VK_ACCESS_SHADER_READ_BIT;

		VkImageSubresourceRange image_range = {};
		image_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		image_range.baseArrayLayer = 0;
		image_range.baseMipLevel = 0;
		image_range.layerCount = 6;
		image_range.levelCount = 1;

		floor_shadow_pipeline_barrier.subresource_range = image_range;

		shadow_pipeline_barriers.push_back(floor_shadow_pipeline_barrier);
	}

	// Create pipelines
	int w, h;

	glfwGetFramebufferSize(renderer.window, &w, &h);

	std::vector<std::pair<std::string, VulkanPipeline>> pipelines;
	VulkanPipeline pipeline_pause_screen = {};
	VulkanPipeline pipeline_death_screen = {};
	VulkanPipeline pipeline_red = {};
	VulkanPipeline pipeline_blue = {};
	VulkanPipeline pipeline_yellow = {};
	VulkanPipeline pipeline_text = {};
	VulkanPipeline pipeline_volume = {};
	VulkanPipeline pipeline_darken = {};
	VulkanPipelineParameters pipeline_parameters = {};
	pipeline_parameters.attribute_descriptions = attribute_descriptions;
	pipeline_parameters.binding_descriptions = binding_descriptions;
	pipeline_parameters.device = renderer.device;
	pipeline_parameters.glfw_window = renderer.window;
	pipeline_parameters.num_textures = 0;
	pipeline_parameters.num_uniform_buffers = 2;
	pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
	pipeline_parameters.pipeline_barriers = {};
	pipeline_parameters.render_pass = render_pass;
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_red.spv"] };
	pipeline_parameters.swap_chain = renderer.swap_chain;
	pipeline_parameters.viewport_width = std::min(w, h);
	pipeline_parameters.viewport_height = std::min(w, h);
	pipeline_parameters.viewport_offset_x = (w - pipeline_parameters.viewport_width) / 2;
	pipeline_parameters.viewport_offset_y = (h - pipeline_parameters.viewport_height) / 2;
	pipeline_parameters.subpass = 0;
	pipeline_parameters.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	pipeline_parameters.num_textures = max_lights;
	pipeline_parameters.num_uniform_buffers += 1;
	pipeline_parameters.access_stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);

	create_pipeline(pipeline_red, pipeline_parameters);
	pipelines.push_back({ "standard_red", pipeline_red });

	pipeline_parameters.num_uniform_buffers -= 1;

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_blue.spv"] };
	pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
	pipeline_parameters.num_textures += 5;

	create_pipeline(pipeline_blue, pipeline_parameters);
	pipelines.push_back({ "standard_blue", pipeline_blue });

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_yellow.spv"] };
	pipeline_parameters.num_textures -= 5;

	create_pipeline(pipeline_yellow, pipeline_parameters);
	pipelines.push_back({ "standard_yellow", pipeline_yellow });

	pipeline_parameters.subpass = 1;
	pipeline_parameters.num_uniform_buffers = 2;
	pipeline_parameters.num_input_attachments = 2;
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_volume.spv"] };
	pipeline_parameters.pipeline_barriers = {};
	pipeline_parameters.samples = VK_SAMPLE_COUNT_1_BIT;

	create_pipeline(pipeline_volume, pipeline_parameters);
	pipelines.push_back({ "volume", pipeline_volume });

	pipeline_parameters.pipeline_barriers = {};
	pipeline_parameters.attribute_descriptions = attribute_descriptions_tex_coords;
	pipeline_parameters.binding_descriptions = binding_descriptions_tex_coords;
	pipeline_parameters.num_textures = 1;
	pipeline_parameters.num_uniform_buffers = 1;
	pipeline_parameters.num_input_attachments = 0;
	pipeline_parameters.subpass = 2;
	pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT };
	pipeline_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_BLEND_ENABLE | PIPELINE_BACKFACE_CULL_DISABLE | PIPELINE_DEPTH_TEST_DISABLE);
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_text.spv"], renderer.data.shaders["Resources/frag_text.spv"] };

	create_pipeline(pipeline_text, pipeline_parameters);

	pipeline_parameters.attribute_descriptions = attribute_descriptions;
	pipeline_parameters.binding_descriptions = binding_descriptions;
	pipeline_parameters.num_textures = 0;
	pipeline_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_BLEND_ENABLE);
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_darken.spv"], renderer.data.shaders["Resources/frag_darken.spv"] };

	create_pipeline(pipeline_darken, pipeline_parameters);

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_menu_screen.spv"], renderer.data.shaders["Resources/frag_pause_screen.spv"] };

	create_pipeline(pipeline_pause_screen, pipeline_parameters);

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_menu_screen.spv"], renderer.data.shaders["Resources/frag_death_screen.spv"] };

	create_pipeline(pipeline_death_screen, pipeline_parameters);

	pipelines.push_back({ "standard_pause", pipeline_pause_screen });
	pipelines.push_back({ "standard_death", pipeline_death_screen });
	pipelines.push_back({ "text", pipeline_text });
	pipelines.push_back({ "darken", pipeline_darken });

	// Create pipelines for shadow maps
	std::vector<std::pair<std::string, VulkanPipeline>> shadow_map_pipelines;
	VulkanPipelineParameters pipeline_shadow_map_parameters = {};
	pipeline_shadow_map_parameters.attribute_descriptions = attribute_descriptions;
	pipeline_shadow_map_parameters.binding_descriptions = binding_descriptions;
	pipeline_shadow_map_parameters.device = renderer.device;
	pipeline_shadow_map_parameters.glfw_window = renderer.window;
	pipeline_shadow_map_parameters.num_textures = 0;
	pipeline_shadow_map_parameters.num_uniform_buffers = 2;
	pipeline_shadow_map_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT };
	pipeline_shadow_map_parameters.pipeline_barriers = {};
	pipeline_shadow_map_parameters.render_pass = shadow_map_render_pass;
	pipeline_shadow_map_parameters.shaders = { renderer.data.shaders["Resources/vert_shadow_map.spv"] };
	pipeline_shadow_map_parameters.swap_chain = renderer.swap_chain;
	pipeline_shadow_map_parameters.viewport_width = 128;
	pipeline_shadow_map_parameters.viewport_height = 128;
	pipeline_shadow_map_parameters.viewport_offset_x = 0;
	pipeline_shadow_map_parameters.viewport_offset_y = 0;
	pipeline_shadow_map_parameters.subpass = 0;
	pipeline_shadow_map_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	pipeline_shadow_map_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_ORDER_CLOCKWISE | PIPELINE_DEPTH_BIAS_ENABLE);

	for (uint32_t i = 0; i < max_lights; i++)
	{
		pipeline_shadow_map_parameters.subpass = i;
		VulkanPipeline shadow_pipeline = {};
		create_pipeline(shadow_pipeline, pipeline_shadow_map_parameters);
		shadow_map_pipelines.push_back({ "SHADOW_" + std::to_string(i), shadow_pipeline });
	}

	// Create reflection map pipelines
	std::vector<std::pair<std::string, VulkanPipeline>> reflection_map_pipelines;
	VulkanPipeline pipeline_red_reflect = {};
	VulkanPipeline pipeline_yellow_reflect = {};
	VulkanPipelineParameters reflect_pipeline_parameters = {};
	reflect_pipeline_parameters.attribute_descriptions = attribute_descriptions;
	reflect_pipeline_parameters.binding_descriptions = binding_descriptions;
	reflect_pipeline_parameters.device = renderer.device;
	reflect_pipeline_parameters.glfw_window = renderer.window;
	reflect_pipeline_parameters.num_textures = max_lights;
	reflect_pipeline_parameters.num_uniform_buffers = 3;
	reflect_pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
	reflect_pipeline_parameters.pipeline_barriers = shadow_pipeline_barriers;
	reflect_pipeline_parameters.render_pass = reflection_map_render_pass;
	reflect_pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_reflect_map.spv"], renderer.data.shaders["Resources/frag_yellow_reflect.spv"] };
	reflect_pipeline_parameters.swap_chain = renderer.swap_chain;
	reflect_pipeline_parameters.viewport_width = 128;
	reflect_pipeline_parameters.viewport_height = 128;
	reflect_pipeline_parameters.viewport_offset_x = 0;
	reflect_pipeline_parameters.viewport_offset_y = 0;
	reflect_pipeline_parameters.subpass = 0;
	reflect_pipeline_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	reflect_pipeline_parameters.pipeline_flags = PIPELINE_ORDER_CLOCKWISE;

	create_pipeline(pipeline_yellow_reflect, reflect_pipeline_parameters);
	reflection_map_pipelines.push_back({ "REFLECT_YELLOW", pipeline_yellow_reflect });

	reflect_pipeline_parameters.pipeline_barriers = {};
	reflect_pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_reflect_map.spv"], renderer.data.shaders["Resources/frag_red_reflect.spv"] };
	reflect_pipeline_parameters.access_stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
	reflect_pipeline_parameters.num_uniform_buffers += 1;

	create_pipeline(pipeline_red_reflect, reflect_pipeline_parameters);
	reflection_map_pipelines.push_back({ "REFLECT_RED", pipeline_red_reflect });

	// Create pipelines for box internals
	std::vector<std::pair<std::string, VulkanPipeline>> box_internals_pipelines = {};
	VulkanPipeline box_internals_pipeline = {};
	VulkanPipelineParameters box_internals_pipeline_parameters = {};
	box_internals_pipeline_parameters.attribute_descriptions = attribute_descriptions;
	box_internals_pipeline_parameters.binding_descriptions = binding_descriptions;
	box_internals_pipeline_parameters.device = renderer.device;
	box_internals_pipeline_parameters.glfw_window = renderer.window;
	box_internals_pipeline_parameters.num_textures = 0;
	box_internals_pipeline_parameters.num_uniform_buffers = 2;
	box_internals_pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
	box_internals_pipeline_parameters.pipeline_barriers = {};
	box_internals_pipeline_parameters.render_pass = box_internals_render_pass;
	box_internals_pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_box_internals.spv"], renderer.data.shaders["Resources/frag_box_internals.spv"] };
	box_internals_pipeline_parameters.swap_chain = renderer.swap_chain;
	box_internals_pipeline_parameters.viewport_width = 64;
	box_internals_pipeline_parameters.viewport_height = 64;
	box_internals_pipeline_parameters.viewport_offset_x = 0;
	box_internals_pipeline_parameters.viewport_offset_y = 0;
	box_internals_pipeline_parameters.subpass = 0;
	box_internals_pipeline_parameters.samples = VK_SAMPLE_COUNT_1_BIT;

	create_pipeline(box_internals_pipeline, box_internals_pipeline_parameters);
	box_internals_pipelines.push_back({ "BOX_INTERNALS", box_internals_pipeline });

	// Create render pass managers
	std::vector<VkClearValue> clear_values(4);
	clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clear_values[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clear_values[2].depthStencil = { 1.0f, 0 };
	clear_values[3].depthStencil = { 1.0f, 0 };

	RenderPassManager render_pass_manager = {};
	RenderPassManagerParameters render_pass_manager_parameters = {};
	render_pass_manager_parameters.pass = render_pass;
	render_pass_manager_parameters.pass_pipelines = pipelines;
	render_pass_manager_parameters.clear_values = clear_values;

	RenderPassManager shadow_map_render_pass_manager = {};
	RenderPassManagerParameters shadow_map_render_pass_manager_parameters = {};
	shadow_map_render_pass_manager_parameters.pass = shadow_map_render_pass;
	shadow_map_render_pass_manager_parameters.pass_pipelines = shadow_map_pipelines;
	shadow_map_render_pass_manager_parameters.clear_values = { { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f } };

	VulkanMipmapGenerationParameters reflection_map_mipmap_parameters = {};
	reflection_map_mipmap_parameters.src_image = renderer.data.textures["REFLECTION_MAP_ATTACHMENT"];
	reflection_map_mipmap_parameters.dst_image = renderer.data.textures["REFLECTION_MAP_FINAL"];

	RenderPassManager reflection_map_render_pass_manager = {};
	RenderPassManagerParameters reflection_map_render_pass_manager_parameters = {};
	reflection_map_render_pass_manager_parameters.pass = reflection_map_render_pass;
	reflection_map_render_pass_manager_parameters.pass_pipelines = reflection_map_pipelines;
	reflection_map_render_pass_manager_parameters.clear_values = { {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f}} };
	reflection_map_render_pass_manager_parameters.mip_map = true;
	reflection_map_render_pass_manager_parameters.mip_parameters = reflection_map_mipmap_parameters;

	VulkanMipmapGenerationParameters box_internals_mipmap_parameters = {};
	box_internals_mipmap_parameters.src_image = renderer.data.textures["BOX_INTERNALS_ATTACHMENT"];
	box_internals_mipmap_parameters.dst_image = renderer.data.textures["BOX_INTERNALS_FINAL"];

	RenderPassManager box_internals_render_pass_manager = {};
	RenderPassManagerParameters box_internals_render_pass_manager_parameters = {};
	box_internals_render_pass_manager_parameters.pass = box_internals_render_pass;
	box_internals_render_pass_manager_parameters.pass_pipelines = box_internals_pipelines;
	box_internals_render_pass_manager_parameters.clear_values = { {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f}} };
	box_internals_render_pass_manager_parameters.mip_map = true;
	box_internals_render_pass_manager_parameters.mip_parameters = box_internals_mipmap_parameters;

	create_render_pass_manager(render_pass_manager, render_pass_manager_parameters);
	create_render_pass_manager(shadow_map_render_pass_manager, shadow_map_render_pass_manager_parameters);
	create_render_pass_manager(reflection_map_render_pass_manager, reflection_map_render_pass_manager_parameters);
	create_render_pass_manager(box_internals_render_pass_manager, box_internals_render_pass_manager_parameters);
	renderer.render_passes = { shadow_map_render_pass_manager, reflection_map_render_pass_manager, box_internals_render_pass_manager, render_pass_manager };

	// Create materials
	Material mat_pause_screen = {};
	mat_pause_screen.models = { &renderer.data.models["SQUARE"] };
	mat_pause_screen.pipelines = { "standard_pause" };
	mat_pause_screen.textures = {};
	mat_pause_screen.use_lights = LIGHT_USAGE_NONE;
	mat_pause_screen.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_pause_screen.pipelines[0]] };
	mat_pause_screen.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_pause_screen.pipelines[0]] };
	mat_pause_screen.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_pause_screen.pipelines[0]] };

	Material mat_death_screen = {};
	mat_death_screen.models = { &renderer.data.models["SQUARE"] };
	mat_death_screen.pipelines = { "standard_death" };
	mat_death_screen.textures = {};
	mat_death_screen.use_lights = LIGHT_USAGE_NONE;
	mat_death_screen.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_death_screen.pipelines[0]] };
	mat_death_screen.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_death_screen.pipelines[0]] };
	mat_death_screen.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_death_screen.pipelines[0]] };

	Material mat_red_square = {};
	mat_red_square.models = { &renderer.data.models["SQUARE"] };
	mat_red_square.pipelines = { "standard_red" };
	mat_red_square.textures = {};
	mat_red_square.use_lights = LIGHT_USAGE_ALL;
	mat_red_square.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_red_square.pipelines[0]] };
	mat_red_square.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_red_square.pipelines[0]] };
	mat_red_square.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_red_square.pipelines[0]] };

	{
		std::string pipeline = "REFLECT_RED";
		mat_red_square.models.push_back(&renderer.data.models["SQUARE"]);
		mat_red_square.pipelines.push_back(pipeline);
		mat_red_square.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].resources[pipeline]);
		mat_red_square.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].vertex_buffers[pipeline]);
		mat_red_square.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].index_buffers[pipeline]);
	}

	for (uint32_t i = 0; i < max_lights; i++)
	{
		std::string pipeline = "SHADOW_" + std::to_string(i);
		mat_red_square.models.push_back(&renderer.data.models["SQUARE"]);
		mat_red_square.pipelines.push_back(pipeline);
		mat_red_square.textures.push_back({ renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)] });
		mat_red_square.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].resources[pipeline]);
		mat_red_square.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].vertex_buffers[pipeline]);
		mat_red_square.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].index_buffers[pipeline]);
	}

	Material mat_blue_cube = {};
	mat_blue_cube.models = { &renderer.data.models["CUBE"] };
	mat_blue_cube.pipelines = { "standard_blue" };
	mat_blue_cube.textures = {};
	mat_blue_cube.use_lights = LIGHT_USAGE_ALL;
	mat_blue_cube.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_blue_cube.pipelines[0]] };
	mat_blue_cube.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_blue_cube.pipelines[0]] };
	mat_blue_cube.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_blue_cube.pipelines[0]] };

	{
		std::string pipeline = "BOX_INTERNALS";
		mat_blue_cube.models.push_back(&renderer.data.models["SQUARE"]);
		mat_blue_cube.pipelines.push_back(pipeline);
		mat_blue_cube.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_BOX_INTERNALS].resources[pipeline]);
		mat_blue_cube.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_BOX_INTERNALS].vertex_buffers[pipeline]);
		mat_blue_cube.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_BOX_INTERNALS].index_buffers[pipeline]);
	}

	for (uint32_t i = 0; i < max_lights; i++)
	{
		std::string pipeline = "SHADOW_" + std::to_string(i);
		mat_blue_cube.models.push_back(&renderer.data.models["CUBE"]);
		mat_blue_cube.pipelines.push_back(pipeline);
		mat_blue_cube.textures.push_back({ renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)] });
		mat_blue_cube.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].resources[pipeline]);
		mat_blue_cube.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].vertex_buffers[pipeline]);
		mat_blue_cube.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].index_buffers[pipeline]);
	}

	mat_blue_cube.textures.push_back({ renderer.data.textures["REFLECTION_MAP_FINAL"] });
	mat_blue_cube.textures.push_back({ renderer.data.textures["BOX_INTERNALS_FINAL"] });
	mat_blue_cube.textures.push_back({ renderer.data.textures["Resources/Roughness_Top.jpg"] });
	mat_blue_cube.textures.push_back({ renderer.data.textures["Resources/Roughness_Horiz.jpg"] });
	mat_blue_cube.textures.push_back({ renderer.data.textures["Resources/Roughness_Vert.jpg"] });

	Material mat_yellow_cube = {};
	mat_yellow_cube.models = { &renderer.data.models["CUBE"] };
	mat_yellow_cube.pipelines = { "standard_yellow" };
	mat_yellow_cube.textures = {  };
	mat_yellow_cube.use_lights = LIGHT_USAGE_ALL;
	mat_yellow_cube.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_yellow_cube.pipelines[0]] };
	mat_yellow_cube.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_yellow_cube.pipelines[0]] };
	mat_yellow_cube.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_yellow_cube.pipelines[0]] };

	{
		std::string pipeline = "REFLECT_YELLOW";
		mat_yellow_cube.models.push_back(&renderer.data.models["CUBE"]);
		mat_yellow_cube.pipelines.push_back(pipeline);
		mat_yellow_cube.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].resources[pipeline]);
		mat_yellow_cube.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].vertex_buffers[pipeline]);
		mat_yellow_cube.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].index_buffers[pipeline]);
	}

	for (uint32_t i = 0; i < max_lights; i++)
	{
		std::string pipeline = "SHADOW_" + std::to_string(i);
		mat_yellow_cube.models.push_back(&renderer.data.models["CUBE"]);
		mat_yellow_cube.pipelines.push_back(pipeline);
		mat_yellow_cube.textures.push_back({ renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)] });
		mat_yellow_cube.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].resources[pipeline]);
		mat_yellow_cube.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].vertex_buffers[pipeline]);
		mat_yellow_cube.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].index_buffers[pipeline]);
	}

	Material mat_text = {};
	mat_text.models = { &renderer.data.models["SQUARE_TEX_COORDS"] };
	mat_text.pipelines = { "text" };
	mat_text.textures = { {renderer.data.textures["Resources/ARIAL.png"]} };;
	mat_text.use_lights = LIGHT_USAGE_NONE;
	mat_text.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_text.pipelines[0]] };
	mat_text.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_text.pipelines[0]] };
	mat_text.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_text.pipelines[0]] };

	Material mat_volume = {};
	mat_volume.models = { &renderer.data.models["SQUARE"] };
	mat_volume.pipelines = { "volume" };
	mat_volume.textures = {};
	mat_volume.input_attachments = { renderer.data.textures["RENDER_PASS_ATTACHMENT_COLOR"], renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"] };
	mat_volume.use_lights = LIGHT_USAGE_ALL;
	mat_volume.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_volume.pipelines[0]] };
	mat_volume.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_volume.pipelines[0]] };
	mat_volume.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_volume.pipelines[0]] };

	for (uint32_t i = 0; i < max_lights; i++)
	{
		mat_volume.textures.push_back({ renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)] });
	}

	Material mat_darken = {};
	mat_darken.models = { &renderer.data.models["SQUARE"] };
	mat_darken.pipelines = { "darken" };
	mat_darken.textures = {};
	mat_darken.input_attachments = { };
	mat_darken.use_lights = LIGHT_USAGE_NONE;
	mat_darken.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_darken.pipelines[0]] };
	mat_darken.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_darken.pipelines[0]] };
	mat_darken.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_darken.pipelines[0]] };

	renderer.data.materials = { mat_pause_screen, mat_death_screen, mat_red_square, mat_blue_cube, mat_yellow_cube, mat_text, mat_volume, mat_darken };

	// Create semaphores/fences
	renderer.image_available_semaphores.resize(parameters.max_frames);
	renderer.render_finished_semaphores.resize(parameters.max_frames);
	renderer.in_flight_fences.resize(parameters.max_frames);
	renderer.images_in_flight.resize(renderer.swap_chain.swap_chain_images.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < parameters.max_frames; i++)
	{
		if (vkCreateSemaphore(renderer.device.device, &semaphore_create_info, nullptr, &renderer.image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(renderer.device.device, &semaphore_create_info, nullptr, &renderer.render_finished_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(renderer.device.device, &fence_create_info, nullptr, &renderer.in_flight_fences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create synchronization objects for a frame!");
		}
	}

	// Create buffer for lights
	renderer.lights.resize(max_lights);
	for (uint32_t i = 0; i < max_lights; i++)
	{
		renderer.lights[i] = {};
	}

	UniformBufferParameters lights_buffer_parameters = {};
	lights_buffer_parameters.size = sizeof(LightUniformBuffer);
	renderer.light_buffers = get_uniform_buffer(renderer, lights_buffer_parameters);

	// Create buffer for shadow maps
	for (uint32_t i = 0; i < max_lights; i++)
	{
		UniformBufferParameters shadow_map_buffer_parameters = {};
		shadow_map_buffer_parameters.range = sizeof(ShadowMapUniformBuffer);
		shadow_map_buffer_parameters.size = sizeof(ShadowMapUniformBuffer);

		renderer.shadow_map_buffers[i] = get_uniform_buffer(renderer, shadow_map_buffer_parameters);
	}

	UniformBufferParameters reflection_map_buffer_parameters = {};
	reflection_map_buffer_parameters.range = sizeof(ReflectionMapUniformBuffer);
	reflection_map_buffer_parameters.size = sizeof(ReflectionMapUniformBuffer);
	renderer.reflection_map_buffer = get_uniform_buffer(renderer, reflection_map_buffer_parameters);

	UniformBufferParameters box_internals_buffer_parameters = {};
	box_internals_buffer_parameters.range = sizeof(BoxInternalsUniformBuffer);
	box_internals_buffer_parameters.size = sizeof(BoxInternalsUniformBuffer);
	renderer.box_internals_buffer = get_uniform_buffer(renderer, box_internals_buffer_parameters);

	UniformBufferParameters volume_buffer_parameters = {};
	volume_buffer_parameters.range = sizeof(VolumeUniformBuffer);
	volume_buffer_parameters.size = sizeof(VolumeUniformBuffer);
	renderer.volume_buffer = get_uniform_buffer(renderer, volume_buffer_parameters);

	InstanceParameters volume_instance_parameters = {};
	volume_instance_parameters.light_index = -1;
	volume_instance_parameters.material = MATERiAL_VOLUME;
	volume_instance_parameters.uniform_buffers = { {renderer.volume_buffer} };

	renderer.volume_instance = create_instance(renderer, volume_instance_parameters);
}

void draw(Renderer &renderer, DrawParameters &parameters)
{
	VolumeUniformBuffer volume_data = {};
	volume_data.model = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0.0, 0.0, -0.5)), glm::vec3(2.071, 2.071, 1.0));
	volume_data.view = glm::lookAt(glm::vec3(0.0, 0.0, 2.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	volume_data.proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);
	volume_data.proj[1][1] *= -1;

	UniformBufferUpdateParameters volume_update_parameters = {};
	volume_update_parameters.buffer_name = renderer.volume_buffer;
	volume_update_parameters.data = &volume_data;

	update_uniform_buffer(renderer, volume_update_parameters);

	InstanceSubmitParameters volume_submit_parameters = {};
	volume_submit_parameters.instance_name = renderer.volume_instance;
	submit_instance(renderer, volume_submit_parameters);

	// Update uniform buffer for creating shadow maps
	glm::mat4 proj = glm::perspective(PI / 2.f, 1.f, 0.001f, 2.0f);

	for (uint8_t j = 0; j < max_lights; j++)
	{
		renderer.shadow_map_uniform.light_index = j;

		glm::vec3 location = renderer.lights[j].location;
		for (uint32_t i = 0; i < 6; i++)
		{
			renderer.shadow_map_uniform.proj[i] = proj;

			switch (i)
			{
			case 4:
				renderer.shadow_map_uniform.view[i] = glm::lookAt(location, glm::vec3(location.x, location.y, location.z + 1.0), glm::vec3(0.0, -1.0, 0.0));
				break;
			case 5:
				renderer.shadow_map_uniform.view[i] = glm::lookAt(location, glm::vec3(location.x, location.y, location.z -1.0), glm::vec3(0.0, -1.0, 0.0));
				break;
			case 2:
				renderer.shadow_map_uniform.view[i] = glm::lookAt(location, glm::vec3(location.x, location.y + 1.0, location.z), glm::vec3(0.0, 0.0, 1.0));
				break;
			case 3:
				renderer.shadow_map_uniform.view[i] = glm::lookAt(location, glm::vec3(location.x, location.y -1.0, location.z), glm::vec3(0.0, 0.0, -1.0));
				break;
			case 0:
				renderer.shadow_map_uniform.view[i] = glm::lookAt(location, glm::vec3(location.x + 1.0, location.y, location.z), glm::vec3(0.0, -1.0, 0.0));
				break;
			case 1:
				renderer.shadow_map_uniform.view[i] = glm::lookAt(location, glm::vec3(location.x -1.0, location.y, location.z), glm::vec3(0.0, -1.0, 0.0));
				break;
			}
		}

		UniformBufferUpdateParameters update_parameters = {};
		update_parameters.buffer_name = renderer.shadow_map_buffers[j];
		update_parameters.data = &renderer.shadow_map_uniform;

		update_uniform_buffer(renderer, update_parameters);
	}

	// Update lights buffer
	LightUniformBuffer lights_data;

	for (uint8_t i = 0; i < max_lights; i++)
	{
		lights_data.active[i].value = renderer.lights[i].active;
		lights_data.color[i].value = renderer.lights[i].color;
		lights_data.intensity[i].value = renderer.lights[i].intensity;
		lights_data.max_distance[i].value = renderer.lights[i].max_distance;
		lights_data.location[i].value = renderer.lights[i].location;
	}

	UniformBufferUpdateParameters lights_buffer_update_parameters = {};
	lights_buffer_update_parameters.buffer_name = renderer.light_buffers;
	lights_buffer_update_parameters.data = &lights_data;
	update_uniform_buffer(renderer, lights_buffer_update_parameters);

	// Record command buffers
	std::vector<std::pair<VulkanRenderPass*, std::vector<Instance*>>> pass_instances;

	for (auto &render_pass : renderer.render_passes)
	{
		// Information grouped according to subpass
		VulkanRenderPassCommandBufferRecordParameters record_parameters = {};
		record_parameters.subpasses.resize(render_pass.pass.total_subpasses);

		for (const auto &pipeline : render_pass.pass_pipelines)
		{
			record_parameters.subpasses[pipeline.second.subpass].vertex_buffers.emplace_back(render_pass.vertex_buffers[pipeline.first]);
			record_parameters.subpasses[pipeline.second.subpass].index_buffers.emplace_back(render_pass.index_buffers[pipeline.first]);
			record_parameters.subpasses[pipeline.second.subpass].resources.push_back(render_pass.resources[pipeline.first]);
			record_parameters.subpasses[pipeline.second.subpass].pipelines.emplace_back(pipeline.second);

			render_pass.vertex_buffers[pipeline.first].clear();
			render_pass.index_buffers[pipeline.first].clear();
			render_pass.resources[pipeline.first].clear();
		}

		record_parameters.device = renderer.device;
		record_parameters.swap_chain = renderer.swap_chain;
		record_parameters.framebuffer_index = renderer.image_index;
		record_parameters.command_index = renderer.image_index;
		record_parameters.clear_values = render_pass.clear_values;

		if (render_pass.pass.framebuffers.size() == 1)
		{
			record_parameters.framebuffer_index = 0;
		}

		if (render_pass.mip_map)
		{
			record_parameters.mip_parameters = render_pass.mip_parameters;
			record_parameters.flags = COMMAND_RECORD_GENERATE_MIPMAPS;
		}

		record_render_pass_command_buffers(render_pass.pass, record_parameters);
	}

	vkWaitForFences(renderer.device.device, 1, &renderer.in_flight_fences[parameters.draw_frame], VK_TRUE, UINT64_MAX);

	if (renderer.images_in_flight[renderer.image_index] != VK_NULL_HANDLE) {
		vkWaitForFences(renderer.device.device, 1, &renderer.images_in_flight[renderer.image_index], VK_TRUE, UINT64_MAX);
	}
	renderer.images_in_flight[renderer.image_index] = renderer.in_flight_fences[parameters.draw_frame];

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = { renderer.image_available_semaphores[parameters.draw_frame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = waitStages;

	std::vector<VkCommandBuffer> command_buffers(renderer.render_passes.size());

	for (uint32_t i = 0; i < renderer.render_passes.size(); i++)
	{
		command_buffers[i] = renderer.render_passes[i].pass.command_buffers[renderer.image_index];
	}

	submit_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());
	submit_info.pCommandBuffers = command_buffers.data();

	VkSemaphore signal_semphores[] = { renderer.render_finished_semaphores[parameters.draw_frame] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semphores;

	vkResetFences(renderer.device.device, 1, &renderer.in_flight_fences[parameters.draw_frame]);

	if (vkQueueSubmit(renderer.device.graphics_queue, 1, &submit_info, renderer.in_flight_fences[parameters.draw_frame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semphores;

	VkSwapchainKHR swap_chains[] = { renderer.swap_chain.swap_chain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;

	present_info.pImageIndices = &renderer.image_index;

	VkResult result = vkQueuePresentKHR(renderer.device.present_queue, &present_info);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR /*|| framebufferResized*/) {

		resize_swap_chain(renderer);
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image!");
	}
}

void update_image_index(Renderer &renderer, uint32_t draw_frame)
{
	// Get image to draw to
	VkResult result = vkAcquireNextImageKHR(renderer.device.device, renderer.swap_chain.swap_chain, UINT64_MAX, renderer.image_available_semaphores[draw_frame], VK_NULL_HANDLE, &renderer.image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		resize_swap_chain(renderer);
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}
}

void update_reflection_map(Renderer &renderer, glm::vec3 location)
{
	// Update reflection map uniform buffer
	{
		ReflectionMapUniformBuffer uniform_data = {};
		uniform_data.proj = glm::perspective(PI / 2.f, 1.f, 0.0015f, 10.0f);

		uniform_data.view[0] = glm::lookAt(location, glm::vec3(location.x + 1.0, location.y, location.z), glm::vec3(0.0, -1.0, 0.0));
		uniform_data.view[1] = glm::lookAt(location, glm::vec3(location.x - 1.0, location.y, location.z), glm::vec3(0.0, -1.0, 0.0));
		uniform_data.view[2] = glm::lookAt(location, glm::vec3(location.x, location.y + 1.0, location.z), glm::vec3(0.0, 0.0, 1.0));
		uniform_data.view[3] = glm::lookAt(location, glm::vec3(location.x, location.y - 1.0, location.z), glm::vec3(0.0, 0.0, -1.0));
		uniform_data.view[4] = glm::lookAt(location, glm::vec3(location.x, location.y, location.z + 1.0), glm::vec3(0.0, -1.0, 0.0));
		uniform_data.view[5] = glm::lookAt(location, glm::vec3(location.x, location.y, location.z - 1.0), glm::vec3(0.0, -1.0, 0.0));

		UniformBufferUpdateParameters update_parameters = {};
		update_parameters.buffer_name = renderer.reflection_map_buffer;
		update_parameters.data = &uniform_data;

		update_uniform_buffer(renderer, update_parameters);
	}

	// Update box internals uniform buffer
	{
		BoxInternalsUniformBuffer uniform_data = {};
		uniform_data.proj = glm::perspective(PI / 2.f, 1.f, 0.0145f, 1.0f);
		uniform_data.proj[1][1] *= -1;

		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.3, 0.3, 1.0));
		glm::mat4 rotate[6];
		rotate[0] = glm::rotate(glm::mat4(1.0f), PI / 2.f, glm::vec3(0.f, 1.f, 0.f));
		rotate[1] = glm::rotate(glm::mat4(1.0f), -PI / 2.f, glm::vec3(0.f, 1.f, 0.f));
		rotate[2] = glm::rotate(glm::mat4(1.0f), -PI / 2.f, glm::vec3(1.f, 0.f, 0.f));
		rotate[3] = glm::rotate(glm::mat4(1.0f), PI / 2.f, glm::vec3(1.f, 0.f, 0.f));
		rotate[4] = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.f, 0.f, 0.f));
		rotate[5] = glm::rotate(glm::mat4(1.0f), PI, glm::vec3(1.f, 0.f, 0.f));

		glm::mat4 translate[6];
		translate[0] = glm::translate(glm::mat4(1.f), glm::vec3(location.x + 0.15, location.y, location.z));
		translate[1] = glm::translate(glm::mat4(1.f), glm::vec3(location.x - 0.15, location.y, location.z));
		translate[2] = glm::translate(glm::mat4(1.f), glm::vec3(location.x, location.y + 0.15, location.z));
		translate[3] = glm::translate(glm::mat4(1.f), glm::vec3(location.x, location.y - 0.15, location.z));
		translate[4] = glm::translate(glm::mat4(1.f), glm::vec3(location.x, location.y, location.z + 0.15));
		translate[5] = glm::translate(glm::mat4(1.f), glm::vec3(location.x, location.y, location.z - 0.15));

		uniform_data.model[0] = translate[0] * rotate[0] * scale;
		uniform_data.model[1] = translate[1] * rotate[1] * scale;
		uniform_data.model[2] = translate[2] * rotate[2] * scale;
		uniform_data.model[3] = translate[3] * rotate[3] * scale;
		uniform_data.model[4] = translate[4] * rotate[4] * scale;
		uniform_data.model[5] = translate[5] * rotate[5] * scale;

		uniform_data.view[0] = glm::lookAt(glm::vec3(location.x + 0.3, location.y, location.z), glm::vec3(location.x - 1.0, location.y, location.z), glm::vec3(0.0, 1.0, 0.0));
		uniform_data.view[1] = glm::lookAt(glm::vec3(location.x - 0.3, location.y, location.z), glm::vec3(location.x + 1.0, location.y, location.z), glm::vec3(0.0, 1.0, 0.0));
		uniform_data.view[2] = glm::lookAt(glm::vec3(location.x, location.y + 0.3, location.z), glm::vec3(location.x, location.y - 1.0, location.z), glm::vec3(0.0, 0.0, -1.0));
		uniform_data.view[3] = glm::lookAt(glm::vec3(location.x, location.y - 0.3, location.z), glm::vec3(location.x, location.y + 1.0, location.z), glm::vec3(0.0, 0.0, 1.0));
		uniform_data.view[4] = glm::lookAt(glm::vec3(location.x, location.y, location.z + 0.3), glm::vec3(location.x, location.y, location.z - 1.0), glm::vec3(0.0, 1.0, 0.0));
		uniform_data.view[5] = glm::lookAt(glm::vec3(location.x, location.y, location.z - 0.3), glm::vec3(location.x, location.y, location.z + 1.0), glm::vec3(0.0, 1.0, 0.0));

		UniformBufferUpdateParameters update_parameters = {};
		update_parameters.buffer_name = renderer.box_internals_buffer;
		update_parameters.data = &uniform_data;

		update_uniform_buffer(renderer, update_parameters);
	}
}

void cleanup_renderer(Renderer &renderer)
{
	vkDeviceWaitIdle(renderer.device.device);

	for (size_t i = 0; i < renderer.max_frames; i++)
	{
		vkDestroySemaphore(renderer.device.device, renderer.render_finished_semaphores[i], nullptr);
		vkDestroySemaphore(renderer.device.device, renderer.image_available_semaphores[i], nullptr);
		vkDestroyFence(renderer.device.device, renderer.in_flight_fences[i], nullptr);
	}

	for (auto render_pass : renderer.render_passes)
	{
		cleanup_render_pass_manager(renderer, render_pass);
	}

	for (auto &instance : renderer.instances)
	{
		for (auto &resource : instance.second.resources)
		{
			cleanup_resource(resource);
		}
	}
	
	cleanup_data_manager(renderer, renderer.data);

	cleanup_swap_chain(renderer.swap_chain);

	cleanup_memory_manager(renderer.memory_manager);

	cleanup_device(renderer.device);

	renderer = {};
}

std::string get_uniform_buffer(Renderer &renderer, UniformBufferParameters &parameters)
{
	// Generate name
	auto num_uniforms = renderer.data.uniform_buffers.size();
	std::string name = "Uniform_Buffer_" + std::to_string(num_uniforms);

	uint32_t range = parameters.range;
	if (range == 0)
	{
		range = parameters.size;
	}

	while (renderer.data.uniform_buffers.find(name) != renderer.data.uniform_buffers.end())
	{
		name += ("_" + std::to_string(num_uniforms));
	}

	// Create buffers
	std::vector<VulkanBuffer> buffers(renderer.swap_chain.swap_chain_images.size());
	VulkanBufferParameters uniform_buffer_parameters = {};
	uniform_buffer_parameters.device = renderer.device;
	uniform_buffer_parameters.memory_manager = &(renderer.memory_manager);
	uniform_buffer_parameters.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	uniform_buffer_parameters.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	uniform_buffer_parameters.size = parameters.size;
	uniform_buffer_parameters.range = range;

	for (uint32_t i = 0; i < buffers.size(); i++)
	{
		create_buffer(buffers[i], uniform_buffer_parameters);
	}

	// Fill out structure
	UniformBuffer uniform_buffer = {};
	uniform_buffer.buffers = buffers;
	uniform_buffer.name = name;

	renderer.data.uniform_buffers[name] = uniform_buffer;

	return name;
}

void update_uniform_buffer(Renderer &renderer, UniformBufferUpdateParameters &parameters)
{
	// Copy data
	VulkanBufferDataParameters data_parameters = {};
	data_parameters.data = parameters.data;
	data_parameters.device = renderer.device;
	data_parameters.size = static_cast<uint32_t>(renderer.data.uniform_buffers[parameters.buffer_name].buffers[renderer.image_index].size);
	data_parameters.offset = 0;
	copy_data_visible_buffer(renderer.data.uniform_buffers[parameters.buffer_name].buffers[renderer.image_index], data_parameters);
}

void free_uniform_buffer(Renderer &renderer, std::string uniform_buffer_name)
{
	UniformBuffer uniform_buffer = renderer.data.uniform_buffers[uniform_buffer_name];
	std::string name = uniform_buffer.name;

	vkDeviceWaitIdle(renderer.device.device);

	for (auto &buffer : uniform_buffer.buffers)
	{
		cleanup_buffer(buffer);
	}

	uniform_buffer = {};

	renderer.data.uniform_buffers.erase(name);
}

std::string create_instance(Renderer &renderer, InstanceParameters &parameters)
{
	// Find pipeline
	Material mat = renderer.data.materials[parameters.material];

	std::vector<VulkanPipeline> chosen_pipelines(mat.pipelines.size());
	uint32_t render_pass_index = 0;
	for (uint32_t k = 0; k < mat.pipelines.size(); k++)
	{
		for (uint32_t i = 0; i < renderer.render_passes.size(); i++)
		{
			const auto &render_pass = renderer.render_passes[i];
			for (const auto &pipeline : render_pass.pass_pipelines)
			{
				if (pipeline.first == mat.pipelines[k])
				{

					chosen_pipelines[k] = pipeline.second;
					render_pass_index = i;
					break;
				}
			}

			if (chosen_pipelines[k].pipeline != VK_NULL_HANDLE)
			{
				break;
			}
		}

		if (chosen_pipelines[k].device == VK_NULL_HANDLE)
		{
			throw std::runtime_error("Could not find pipeline!");
		}
	}

	// Generate name
	std::string name = std::to_string(parameters.material);

	for (const auto &uniform_buffers : parameters.uniform_buffers)
	{
		for (const auto &uniform_buffer : uniform_buffers)
		name += "_" + uniform_buffer;
	}

	// Make sure resource of this description doesn't already exist
	for (const auto &render_pass : renderer.render_passes)
	{
		if (render_pass.resources.find(name) != render_pass.resources.end())
		{
			throw std::runtime_error("Tried to create resource which already exists!");
		}
	}

	// Fill out structure
	std::vector<VulkanResource> resources;
	VulkanResource resource = {};
	VulkanResourceParameters resource_parameters = {};
	resource_parameters.device = renderer.device;
	resource_parameters.swap_chain = renderer.swap_chain;
	uint32_t shadow_index = 0;

	// NOTE: chosen_pipelines (Minus shadow map pipelines) should be the same size as parameters.uniform_buffers
	for (uint32_t i = 0; i < mat.pipelines.size(); i++)
	{
		resource_parameters.pipeline = chosen_pipelines[i];
		resource_parameters.textures = mat.textures;
		resource_parameters.uniform_buffers = {};
		resource_parameters.input_attachments = mat.input_attachments;

		// If lights are not being used
		if (mat.use_lights == LIGHT_USAGE_NONE)
		{
			// Submit just the normal uniform buffers
			for (uint32_t j = 0; j < parameters.uniform_buffers[i].size(); j++)
			{
				resource_parameters.uniform_buffers.push_back(renderer.data.uniform_buffers[parameters.uniform_buffers[i][j]].buffers);
			}
		}
		else if (mat.use_lights == LIGHT_USAGE_ALL && mat.pipelines[i].substr(0, 6) != "SHADOW")
		{
			// Otherwise also use the uniform buffer with lighting information
			for (uint32_t j = 0; j < parameters.uniform_buffers[i].size(); j++)
			{
				resource_parameters.uniform_buffers.push_back(renderer.data.uniform_buffers[parameters.uniform_buffers[i][j]].buffers);
			}
			resource_parameters.uniform_buffers.push_back({ renderer.data.uniform_buffers[renderer.light_buffers].buffers });
		}
		else if (mat.pipelines[i].substr(0, 6) == "SHADOW")
		{
			// If this pipeline is for shadow maps, use that uniform buffer
			resource_parameters.uniform_buffers = { (renderer.data.uniform_buffers[parameters.uniform_buffers[0][0]].buffers) ,  renderer.data.uniform_buffers[renderer.shadow_map_buffers[shadow_index]].buffers };
			shadow_index++;
			resource_parameters.textures = {};
		}

		// If this pipeline is reflecting, add the reflection ubo
		if (mat.pipelines[i].substr(0, 7) == "REFLECT")
		{
			resource_parameters.uniform_buffers.insert(resource_parameters.uniform_buffers.begin() + 1, renderer.data.uniform_buffers[renderer.reflection_map_buffer].buffers);
		}
		else if (mat.pipelines[i] == "BOX_INTERNALS")
		{
			resource_parameters.uniform_buffers[0] = renderer.data.uniform_buffers[renderer.box_internals_buffer].buffers;
			resource_parameters.textures = {};
		}

		create_resource(resource, resource_parameters);
		resources.push_back(resource);
	}

	Instance instance = {};
	instance.resources = resources;
	instance.material = parameters.material;

	renderer.instances[name] = instance;
	return name;
}

void submit_instance(Renderer &renderer, InstanceSubmitParameters &parameters)
{
	const Instance &instance = renderer.instances[parameters.instance_name];
	Material &material = renderer.data.materials[instance.material];

	// Note: instance.resources must be the same size as material.resources
	for (uint32_t i = 0; i < material.resources.size(); i++)
	{
		material.resources[i]->push_back(instance.resources[i]);
		material.vertex_buffers[i]->push_back(material.models[i]->first);
		material.index_buffers[i]->push_back(material.models[i]->second);
	}
}

void free_instance(Renderer &renderer, std::string instance_name)
{
	auto &instance = renderer.instances[instance_name];
	for (auto &resource : instance.resources)
	{
		cleanup_resource(resource);
	}
	renderer.instances.erase(instance_name);
}

uint8_t create_light(Renderer &renderer, LightParameters &parameters)
{
	// Initialize value (so compiler doesn't complain)
	Light *light = &renderer.lights[0];
	uint8_t light_index = max_lights + 1;

	// Find inactive light
	for (uint8_t i = 0; i < max_lights; i++)
	{
		if (!renderer.lights[i].active)
		{
			light = &renderer.lights[i];
			light_index = i;
			break;
		}
	}

	if (light_index == max_lights + 1)
	{
		throw std::runtime_error("Tried to create more lights than are available!");
	}

	// Set light values
	light->active = 1;
	light->color = parameters.color;
	light->intensity = parameters.intensity;
	light->max_distance = parameters.max_distance;
	light->location = parameters.location;
	light->type = parameters.type;

	return light_index;
}

void update_light(Renderer &renderer, LightUpdateParameters &parameters)
{
	// Retrieve light
	Light *light = &renderer.lights[parameters.light_index];

	if (!light->active)
	{
		throw std::runtime_error("Tried to update an inactive light!");
	}
	
	// Set values
	light->color = parameters.color;
	light->intensity = parameters.intensity;
	light->max_distance = parameters.max_distance;
	light->location = parameters.location;
}

void free_light(Renderer &renderer, uint8_t light_index)
{
	// Retrieve light
	Light *light = &renderer.lights[light_index];

	if (!light->active)
	{
		throw std::runtime_error("Tried to free an inactive light!");
	}

	// Set inactive
	light->active = 0;
}

void create_data_manager(DataManager &data_manager, DataManagerParameters &data_manager_parameters)
{
	// Copy over objects
	data_manager.models = data_manager_parameters.models;
	data_manager.shaders = data_manager_parameters.shaders;
	data_manager.textures = data_manager_parameters.textures;
	data_manager.uniform_buffers = data_manager_parameters.uniform_buffers;
	data_manager.materials = data_manager_parameters.materials;
}

void cleanup_data_manager(Renderer &renderer, DataManager &data_manager)
{
	std::vector<VulkanBuffer> buffers_to_cleanup = {};
	for (auto &model : data_manager.models)
	{
		buffers_to_cleanup.push_back(model.second.first);
		buffers_to_cleanup.push_back(model.second.second);

		for (int i = 0; i < static_cast<int>(buffers_to_cleanup.size()) - 2; i++)
		{
			if (buffers_to_cleanup[i].buffer == model.second.first.buffer)
			{
				buffers_to_cleanup.erase(buffers_to_cleanup.begin() + i--);
			}
			if (buffers_to_cleanup[i].buffer == model.second.second.buffer)
			{
				buffers_to_cleanup.erase(buffers_to_cleanup.begin() + i--);
			}
		}
	}
	for (auto &buffer : buffers_to_cleanup)
	{
		cleanup_buffer(buffer);
	}

	for (auto &shader : data_manager.shaders)
	{
		cleanup_shader(shader.second);
	}

	for (auto &texture : data_manager.textures)
	{
		cleanup_texture(texture.second);
	}

	for (auto &uniform_buffer : data_manager.uniform_buffers)
	{
		std::string name = uniform_buffer.second.name;

		for (auto &buffer : uniform_buffer.second.buffers)
		{
			cleanup_buffer(buffer);
		}

		uniform_buffer.second = {};
	}

	data_manager = {};
}

void create_render_pass_manager(RenderPassManager &render_pass_manager, RenderPassManagerParameters &render_pass_manager_parameters)
{
	render_pass_manager.pass = render_pass_manager_parameters.pass;
	render_pass_manager.pass_pipelines = render_pass_manager_parameters.pass_pipelines;
	render_pass_manager.resources = {};
	render_pass_manager.vertex_buffers = {};
	render_pass_manager.index_buffers = {};
	render_pass_manager.clear_values = render_pass_manager_parameters.clear_values;
	render_pass_manager.mip_map = render_pass_manager_parameters.mip_map;
	render_pass_manager.mip_parameters = render_pass_manager_parameters.mip_parameters;
}

void cleanup_render_pass_manager(Renderer &renderer, RenderPassManager &render_pass_manager)
{
	vkDeviceWaitIdle(render_pass_manager.pass.device);
	cleanup_render_pass_command_buffers(render_pass_manager.pass);

	for (auto &pipeline : render_pass_manager.pass_pipelines)
	{
		cleanup_pipeline(pipeline.second);
	}

	cleanup_render_pass(render_pass_manager.pass);

	render_pass_manager = {};
}

void resize_swap_chain(Renderer &renderer)
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(renderer.window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(renderer.window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(renderer.device.device);

	for (uint32_t i = 0; i < renderer.render_passes.size(); i++)
	{
		cleanup_render_pass_manager(renderer, renderer.render_passes[i]);
	}

	cleanup_swap_chain(renderer.swap_chain);

	// Create swap chain
	VulkanSwapChainParameters swap_chain_parameters = {};
	swap_chain_parameters.device = renderer.device;
	swap_chain_parameters.glfw_window = renderer.window;

	create_swap_chain(renderer.swap_chain, swap_chain_parameters);

	// Recreate attachments
	cleanup_texture(renderer.data.textures["RENDER_PASS_ATTACHMENT_COLOR"]);
	cleanup_texture(renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"]);
	cleanup_texture(renderer.data.textures["RENDER_PASS_ATTACHMENT_VOLUME_DEPTH"]);

	// Create textures for framebuffer attachments
	VulkanTexture color_attachment = {};
	VulkanTextureParameters color_attachment_parameters = {};
	color_attachment_parameters.device = renderer.device;
	color_attachment_parameters.command_pool = renderer.device.command_pool;
	color_attachment_parameters.memory_manager = &renderer.memory_manager;
	color_attachment_parameters.format = renderer.swap_chain.swap_chain_format;
	color_attachment_parameters.height = renderer.swap_chain.swap_chain_extent.height;
	color_attachment_parameters.width = renderer.swap_chain.swap_chain_extent.width;
	color_attachment_parameters.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	color_attachment_parameters.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	create_texture(color_attachment, color_attachment_parameters);

	renderer.data.textures["RENDER_PASS_ATTACHMENT_COLOR"] = color_attachment;

	VulkanTexture depth_attachment = {};
	VulkanTextureParameters depth_attachment_parameters = {};
	depth_attachment_parameters.device = renderer.device;
	depth_attachment_parameters.command_pool = renderer.device.command_pool;
	depth_attachment_parameters.memory_manager = &renderer.memory_manager;
	depth_attachment_parameters.format = find_depth_format(renderer.device.physical_device);
	depth_attachment_parameters.height = renderer.swap_chain.swap_chain_extent.height;
	depth_attachment_parameters.width = renderer.swap_chain.swap_chain_extent.width;
	depth_attachment_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	depth_attachment_parameters.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	create_texture(depth_attachment, depth_attachment_parameters);

	renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"] = depth_attachment;

	VulkanTexture volume_depth_attachment = {};
	VulkanTextureParameters volume_depth_attachment_parameters = {};
	volume_depth_attachment_parameters.device = renderer.device;
	volume_depth_attachment_parameters.command_pool = renderer.device.command_pool;
	volume_depth_attachment_parameters.memory_manager = &renderer.memory_manager;
	volume_depth_attachment_parameters.format = find_depth_format(renderer.device.physical_device);
	volume_depth_attachment_parameters.height = renderer.swap_chain.swap_chain_extent.height;
	volume_depth_attachment_parameters.width = renderer.swap_chain.swap_chain_extent.width;
	volume_depth_attachment_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	volume_depth_attachment_parameters.samples = VK_SAMPLE_COUNT_1_BIT;

	create_texture(volume_depth_attachment, volume_depth_attachment_parameters);

	renderer.data.textures["RENDER_PASS_ATTACHMENT_VOLUME_DEPTH"] = volume_depth_attachment;

	// Create render passes
	VulkanRenderPass render_pass = {};
	VulkanRenderPassParameters render_pass_parameters = {};
	render_pass_parameters.device = renderer.device;
	render_pass_parameters.glfw_window = renderer.window;
	render_pass_parameters.memory_manager = &(renderer.memory_manager);
	render_pass_parameters.swap_chain = renderer.swap_chain;

	// Description for swap chain attachment
	VulkanRenderPassAttachment swap_chain_attachment_description = {};
	swap_chain_attachment_description.attachment_format = renderer.swap_chain.swap_chain_format;
	swap_chain_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	swap_chain_attachment_description.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	swap_chain_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	// Description for color attachment (Before downsampling)
	VulkanRenderPassAttachment color_attachment_description = {};
	color_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_COLOR"];
	color_attachment_description.attachment_format = color_attachment_description.attachment.format;
	color_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	color_attachment_description.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	// Description for depth attachment
	VulkanRenderPassAttachment depth_attachment_description = {};
	depth_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"];
	depth_attachment_description.attachment_format = depth_attachment_description.attachment.format;
	depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment_description.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	// Description for volumetric lighting depth attachment
	VulkanRenderPassAttachment volume_depth_attachment_description = {};
	volume_depth_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_VOLUME_DEPTH"];
	volume_depth_attachment_description.attachment_format = volume_depth_attachment_description.attachment.format;
	volume_depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	volume_depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	volume_depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	// Dependency between volumetric lighting subpass and text subpass
	VkSubpassDependency volume_text_dependency = {};
	volume_text_dependency.srcSubpass = 1;
	volume_text_dependency.dstSubpass = 2;
	volume_text_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	volume_text_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	volume_text_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	volume_text_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	volume_text_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Dependency between opaque subpass and volume subpass
	VkSubpassDependency opaque_volume_color_dependency = {};
	opaque_volume_color_dependency.srcSubpass = 0;
	opaque_volume_color_dependency.dstSubpass = 1;
	opaque_volume_color_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	opaque_volume_color_dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	opaque_volume_color_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	opaque_volume_color_dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	opaque_volume_color_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency opaque_volume_depth_dependency = {};
	opaque_volume_depth_dependency.srcSubpass = 0;
	opaque_volume_depth_dependency.dstSubpass = 1;
	opaque_volume_depth_dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	opaque_volume_depth_dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	opaque_volume_depth_dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	opaque_volume_depth_dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	opaque_volume_depth_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Description of opaque subpass
	VulkanRenderPassSubpassDescription subpass_opaque_description = {};
	subpass_opaque_description.color_attachments = { 1 };
	subpass_opaque_description.depth_attachment = 2;
	subpass_opaque_description.use_depth = true;
	subpass_opaque_description.dependencies = {};

	// Description of volumetric lighting subpass
	VulkanRenderPassSubpassDescription subpass_volume_description = {};
	subpass_volume_description.color_attachments = { 0 };
	subpass_volume_description.depth_attachment = 3;
	subpass_volume_description.input_attachments = { 1, 2 };
	subpass_volume_description.use_depth = true;
	subpass_volume_description.dependencies = { opaque_volume_color_dependency, opaque_volume_depth_dependency };

	// Description of text subpass
	VulkanRenderPassSubpassDescription subpass_text_description = {};
	subpass_text_description.color_attachments = { 0 };
	subpass_text_description.depth_attachment = { 3 };
	subpass_text_description.use_depth = true;
	subpass_text_description.dependencies = { volume_text_dependency };

	VulkanRenderPassSubpasses subpasses = {};
	subpasses.attachments = { swap_chain_attachment_description, color_attachment_description, depth_attachment_description, volume_depth_attachment_description };
	subpasses.subpass_descriptions = { subpass_opaque_description, subpass_volume_description, subpass_text_description };

	render_pass_parameters.subpasses = subpasses;

	create_render_pass(render_pass, render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters command_buffer_parameters = {};
	command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(render_pass, command_buffer_parameters);

	// Create shadow map subpass and attachment descriptions
	std::vector<VulkanRenderPassAttachment> shadow_map_attachment_descriptions = {};
	std::vector<VulkanRenderPassSubpassDescription> shadow_map_subpass_descriptions = {};
	for (uint32_t i = 0; i < max_lights; i++)
	{
		VulkanRenderPassAttachment shadow_map_attachment_description = {};
		shadow_map_attachment_description.attachment = renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)];
		shadow_map_attachment_description.attachment_format = shadow_map_attachment_description.attachment.format;
		shadow_map_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		shadow_map_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		shadow_map_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		shadow_map_attachment_descriptions.push_back(shadow_map_attachment_description);

		VulkanRenderPassSubpassDescription shadow_map_subpass_description = {};
		shadow_map_subpass_description.depth_attachment = i;
		shadow_map_subpass_description.use_depth = true;
		shadow_map_subpass_description.color_attachments = {};
		shadow_map_subpass_description.input_attachments = {};
		shadow_map_subpass_description.resolve_attachments = {};
		shadow_map_subpass_description.dependencies = {};
		shadow_map_subpass_descriptions.push_back(shadow_map_subpass_description);
	}

	VulkanRenderPassSubpasses shadow_map_subpasses = {};
	shadow_map_subpasses.attachments = shadow_map_attachment_descriptions;
	shadow_map_subpasses.subpass_descriptions = shadow_map_subpass_descriptions;

	// Create render pass for shadow maps
	VulkanRenderPass shadow_map_render_pass = {};
	VulkanRenderPassParameters shadow_map_render_pass_parameters = {};
	shadow_map_render_pass_parameters.device = renderer.device;
	shadow_map_render_pass_parameters.glfw_window = renderer.window;
	shadow_map_render_pass_parameters.memory_manager = &renderer.memory_manager;
	shadow_map_render_pass_parameters.swap_chain = renderer.swap_chain;
	shadow_map_render_pass_parameters.subpasses = shadow_map_subpasses;
	shadow_map_render_pass_parameters.flags = static_cast<RenderPassFlags>(RENDER_PASS_IGNORE_DRAW_IMAGES | RENDER_PASS_MULTIVIEW);
	shadow_map_render_pass_parameters.num_views = 6;
	shadow_map_render_pass_parameters.view_masks = std::vector<uint32_t>(14, 0b00111111);

	create_render_pass(shadow_map_render_pass, shadow_map_render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters shadow_map_command_buffer_parameters = {};
	shadow_map_command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(shadow_map_render_pass, shadow_map_command_buffer_parameters);

	// Create reflection map attachments and subpasses
	VulkanRenderPassAttachment reflection_map_attachment_description = {};
	reflection_map_attachment_description.attachment = renderer.data.textures["REFLECTION_MAP_ATTACHMENT"];
	reflection_map_attachment_description.attachment_format = reflection_map_attachment_description.attachment.format;
	reflection_map_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	reflection_map_attachment_description.final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	reflection_map_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassAttachment reflection_map_depth_attachment_description = {};
	reflection_map_depth_attachment_description.attachment = renderer.data.textures["REFLECTION_MAP_DEPTH_ATTACHMENT"];
	reflection_map_depth_attachment_description.attachment_format = reflection_map_depth_attachment_description.attachment.format;
	reflection_map_depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	reflection_map_depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	reflection_map_depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassSubpassDescription reflection_map_subpass_description = {};
	reflection_map_subpass_description.color_attachments = { 0 };
	reflection_map_subpass_description.depth_attachment = 1;
	reflection_map_subpass_description.use_depth = true;

	VulkanRenderPassSubpasses reflection_map_subpasses = {};
	reflection_map_subpasses.attachments = { reflection_map_attachment_description, reflection_map_depth_attachment_description };
	reflection_map_subpasses.subpass_descriptions = { reflection_map_subpass_description };

	// Create reflection map render pass
	VulkanRenderPass reflection_map_render_pass = {};
	VulkanRenderPassParameters reflection_map_render_pass_parameters = {};
	reflection_map_render_pass_parameters.device = renderer.device;
	reflection_map_render_pass_parameters.glfw_window = renderer.window;
	reflection_map_render_pass_parameters.memory_manager = &renderer.memory_manager;
	reflection_map_render_pass_parameters.swap_chain = renderer.swap_chain;
	reflection_map_render_pass_parameters.subpasses = reflection_map_subpasses;
	reflection_map_render_pass_parameters.flags = static_cast<RenderPassFlags>(RENDER_PASS_IGNORE_DRAW_IMAGES | RENDER_PASS_MULTIVIEW);
	reflection_map_render_pass_parameters.num_views = 6;
	reflection_map_render_pass_parameters.view_masks = std::vector<uint32_t>(14, 0b00111111);

	create_render_pass(reflection_map_render_pass, reflection_map_render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters reflection_map_command_buffer_parameters = {};
	reflection_map_command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(reflection_map_render_pass, reflection_map_command_buffer_parameters);

	// Create box internals attachments and subpasses
	VulkanRenderPassAttachment box_internals_attachment_description = {};
	box_internals_attachment_description.attachment = renderer.data.textures["BOX_INTERNALS_ATTACHMENT"];
	box_internals_attachment_description.attachment_format = box_internals_attachment_description.attachment.format;
	box_internals_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	box_internals_attachment_description.final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	box_internals_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassAttachment box_internals_depth_attachment_description = {};
	box_internals_depth_attachment_description.attachment = renderer.data.textures["BOX_INTERNALS_DEPTH_ATTACHMENT"];
	box_internals_depth_attachment_description.attachment_format = box_internals_depth_attachment_description.attachment.format;
	box_internals_depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	box_internals_depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	box_internals_depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassSubpassDescription box_internals_subpass_description = {};
	box_internals_subpass_description.color_attachments = { 0 };
	box_internals_subpass_description.depth_attachment = 1;
	box_internals_subpass_description.use_depth = true;

	VulkanRenderPassSubpasses box_internals_subpasses = {};
	box_internals_subpasses.attachments = { box_internals_attachment_description, box_internals_depth_attachment_description };
	box_internals_subpasses.subpass_descriptions = { box_internals_subpass_description };

	// Create box internals render pass
	VulkanRenderPass box_internals_render_pass = {};
	VulkanRenderPassParameters box_internals_render_pass_parameters = {};
	box_internals_render_pass_parameters.device = renderer.device;
	box_internals_render_pass_parameters.glfw_window = renderer.window;
	box_internals_render_pass_parameters.memory_manager = &renderer.memory_manager;
	box_internals_render_pass_parameters.swap_chain = renderer.swap_chain;
	box_internals_render_pass_parameters.subpasses = box_internals_subpasses;
	box_internals_render_pass_parameters.flags = static_cast<RenderPassFlags>(RENDER_PASS_IGNORE_DRAW_IMAGES | RENDER_PASS_MULTIVIEW);
	box_internals_render_pass_parameters.num_views = 6;
	box_internals_render_pass_parameters.view_masks = std::vector<uint32_t>(14, 0b00111111);

	create_render_pass(box_internals_render_pass, box_internals_render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters box_internals_command_buffer_parameters = {};
	box_internals_command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(box_internals_render_pass, box_internals_command_buffer_parameters);

	// Create attribute and binding description
	struct VertexWithTexCoord
	{
		alignas(16) glm::vec3 point;
		alignas(8) glm::vec2 tex_coord;
	};

	struct VertexWithNormal
	{
		alignas(16) glm::vec3 point;
		alignas(16) glm::vec3 normal;
	};

	std::unordered_map<std::string, std::pair<VulkanBuffer, VulkanBuffer>> models = {};
	VkVertexInputBindingDescription binding_description = {};
	binding_description.binding = 0;
	binding_description.stride = sizeof(VertexWithNormal);
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputBindingDescription> binding_descriptions = { binding_description };

	VkVertexInputBindingDescription binding_description_tex_coord = {};
	binding_description_tex_coord.binding = 0;
	binding_description_tex_coord.stride = sizeof(VertexWithTexCoord);
	binding_description_tex_coord.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputBindingDescription> binding_descriptions_tex_coords = { binding_description_tex_coord };

	VkVertexInputAttributeDescription attribute_description = {};

	attribute_description.binding = 0;
	attribute_description.location = 0;
	attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description.offset = offsetof(VertexWithNormal, point);

	VkVertexInputAttributeDescription attribute_description_normal = {};

	attribute_description_normal.binding = 0;
	attribute_description_normal.location = 1;
	attribute_description_normal.format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description_normal.offset = offsetof(VertexWithNormal, normal);

	VkVertexInputAttributeDescription attribute_description_tex_coord_location = {};

	attribute_description_tex_coord_location.binding = 0;
	attribute_description_tex_coord_location.location = 0;
	attribute_description_tex_coord_location.format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description_tex_coord_location.offset = offsetof(VertexWithTexCoord, point);

	VkVertexInputAttributeDescription attribute_description_tex_coord_coord = {};

	attribute_description_tex_coord_coord.binding = 0;
	attribute_description_tex_coord_coord.location = 1;
	attribute_description_tex_coord_coord.format = VK_FORMAT_R32G32_SFLOAT;
	attribute_description_tex_coord_coord.offset = offsetof(VertexWithTexCoord, tex_coord);
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions_tex_coords = { attribute_description_tex_coord_location, attribute_description_tex_coord_coord };
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions = { attribute_description, attribute_description_normal };

	// Create pipeline barriers
	std::vector<VulkanPipelineBarrier> shadow_pipeline_barriers = {};

	for (uint32_t i = 0; i < max_lights; i++)
	{
		VulkanPipelineBarrier floor_shadow_pipeline_barrier = {};
		floor_shadow_pipeline_barrier.images = std::vector<VulkanTexture>(renderer.swap_chain.swap_chain_images.size(), renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)]);
		floor_shadow_pipeline_barrier.old_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		floor_shadow_pipeline_barrier.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		floor_shadow_pipeline_barrier.src = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		floor_shadow_pipeline_barrier.dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		floor_shadow_pipeline_barrier.src_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		floor_shadow_pipeline_barrier.dst_access = VK_ACCESS_SHADER_READ_BIT;

		VkImageSubresourceRange image_range = {};
		image_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		image_range.baseArrayLayer = 0;
		image_range.baseMipLevel = 0;
		image_range.layerCount = 6;
		image_range.levelCount = 1;

		floor_shadow_pipeline_barrier.subresource_range = image_range;

		shadow_pipeline_barriers.push_back(floor_shadow_pipeline_barrier);
	}

	// Create pipelines
	int w, h;

	glfwGetFramebufferSize(renderer.window, &w, &h);

	std::vector<std::pair<std::string, VulkanPipeline>> pipelines;
	VulkanPipeline pipeline_pause_screen = {};
	VulkanPipeline pipeline_death_screen = {};
	VulkanPipeline pipeline_red = {};
	VulkanPipeline pipeline_blue = {};
	VulkanPipeline pipeline_yellow = {};
	VulkanPipeline pipeline_text = {};
	VulkanPipeline pipeline_volume = {};
	VulkanPipeline pipeline_darken = {};
	VulkanPipelineParameters pipeline_parameters = {};
	pipeline_parameters.attribute_descriptions = attribute_descriptions;
	pipeline_parameters.binding_descriptions = binding_descriptions;
	pipeline_parameters.device = renderer.device;
	pipeline_parameters.glfw_window = renderer.window;
	pipeline_parameters.num_textures = 0;
	pipeline_parameters.num_uniform_buffers = 2;
	pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
	pipeline_parameters.pipeline_barriers = {};
	pipeline_parameters.render_pass = render_pass;
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_red.spv"] };
	pipeline_parameters.swap_chain = renderer.swap_chain;
	pipeline_parameters.viewport_width = std::min(w, h);
	pipeline_parameters.viewport_height = std::min(w, h);
	pipeline_parameters.viewport_offset_x = (w - pipeline_parameters.viewport_width) / 2;
	pipeline_parameters.viewport_offset_y = (h - pipeline_parameters.viewport_height) / 2;
	pipeline_parameters.subpass = 0;
	pipeline_parameters.samples = /*renderer.device.max_sample_count*/ VK_SAMPLE_COUNT_4_BIT;

	pipeline_parameters.num_textures = max_lights;
	pipeline_parameters.num_uniform_buffers += 1;
	pipeline_parameters.access_stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);

	create_pipeline(pipeline_red, pipeline_parameters);
	pipelines.push_back({ "standard_red", pipeline_red });

	pipeline_parameters.num_uniform_buffers -= 1;

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_blue.spv"] };
	pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
	pipeline_parameters.num_textures += 5;

	create_pipeline(pipeline_blue, pipeline_parameters);
	pipelines.push_back({ "standard_blue", pipeline_blue });

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_yellow.spv"] };
	pipeline_parameters.num_textures -= 5;

	create_pipeline(pipeline_yellow, pipeline_parameters);
	pipelines.push_back({ "standard_yellow", pipeline_yellow });

	pipeline_parameters.subpass = 1;
	pipeline_parameters.num_uniform_buffers = 2;
	pipeline_parameters.num_input_attachments = 2;
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_volume.spv"] };
	pipeline_parameters.pipeline_barriers = {};
	pipeline_parameters.samples = VK_SAMPLE_COUNT_1_BIT;

	create_pipeline(pipeline_volume, pipeline_parameters);
	pipelines.push_back({ "volume", pipeline_volume });

	pipeline_parameters.pipeline_barriers = {};
	pipeline_parameters.attribute_descriptions = attribute_descriptions_tex_coords;
	pipeline_parameters.binding_descriptions = binding_descriptions_tex_coords;
	pipeline_parameters.num_textures = 1;
	pipeline_parameters.num_uniform_buffers = 1;
	pipeline_parameters.num_input_attachments = 0;
	pipeline_parameters.subpass = 2;
	pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT };
	pipeline_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_BLEND_ENABLE | PIPELINE_BACKFACE_CULL_DISABLE | PIPELINE_DEPTH_TEST_DISABLE);
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_text.spv"], renderer.data.shaders["Resources/frag_text.spv"] };

	create_pipeline(pipeline_text, pipeline_parameters);

	pipeline_parameters.attribute_descriptions = attribute_descriptions;
	pipeline_parameters.binding_descriptions = binding_descriptions;
	pipeline_parameters.num_textures = 0;
	pipeline_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_BLEND_ENABLE);
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_darken.spv"], renderer.data.shaders["Resources/frag_darken.spv"] };

	create_pipeline(pipeline_darken, pipeline_parameters);

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_menu_screen.spv"], renderer.data.shaders["Resources/frag_pause_screen.spv"] };

	create_pipeline(pipeline_pause_screen, pipeline_parameters);

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_menu_screen.spv"], renderer.data.shaders["Resources/frag_death_screen.spv"] };

	create_pipeline(pipeline_death_screen, pipeline_parameters);

	pipelines.push_back({ "standard_pause", pipeline_pause_screen });
	pipelines.push_back({ "standard_death", pipeline_death_screen });
	pipelines.push_back({ "text", pipeline_text });
	pipelines.push_back({ "darken", pipeline_darken });

	// Create pipelines for shadow maps
	std::vector<std::pair<std::string, VulkanPipeline>> shadow_map_pipelines;
	VulkanPipelineParameters pipeline_shadow_map_parameters = {};
	pipeline_shadow_map_parameters.attribute_descriptions = attribute_descriptions;
	pipeline_shadow_map_parameters.binding_descriptions = binding_descriptions;
	pipeline_shadow_map_parameters.device = renderer.device;
	pipeline_shadow_map_parameters.glfw_window = renderer.window;
	pipeline_shadow_map_parameters.num_textures = 0;
	pipeline_shadow_map_parameters.num_uniform_buffers = 2;
	pipeline_shadow_map_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT };
	pipeline_shadow_map_parameters.pipeline_barriers = {};
	pipeline_shadow_map_parameters.render_pass = shadow_map_render_pass;
	pipeline_shadow_map_parameters.shaders = { renderer.data.shaders["Resources/vert_shadow_map.spv"] };
	pipeline_shadow_map_parameters.swap_chain = renderer.swap_chain;
	pipeline_shadow_map_parameters.viewport_width = 128;
	pipeline_shadow_map_parameters.viewport_height = 128;
	pipeline_shadow_map_parameters.viewport_offset_x = 0;
	pipeline_shadow_map_parameters.viewport_offset_y = 0;
	pipeline_shadow_map_parameters.subpass = 0;
	pipeline_shadow_map_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	pipeline_shadow_map_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_ORDER_CLOCKWISE | PIPELINE_DEPTH_BIAS_ENABLE);

	for (uint32_t i = 0; i < max_lights; i++)
	{
		pipeline_shadow_map_parameters.subpass = i;
		VulkanPipeline shadow_pipeline = {};
		create_pipeline(shadow_pipeline, pipeline_shadow_map_parameters);
		shadow_map_pipelines.push_back({ "SHADOW_" + std::to_string(i), shadow_pipeline });
	}

	// Create reflection map pipelines
	std::vector<std::pair<std::string, VulkanPipeline>> reflection_map_pipelines;
	VulkanPipeline pipeline_red_reflect = {};
	VulkanPipeline pipeline_yellow_reflect = {};
	VulkanPipelineParameters reflect_pipeline_parameters = {};
	reflect_pipeline_parameters.attribute_descriptions = attribute_descriptions;
	reflect_pipeline_parameters.binding_descriptions = binding_descriptions;
	reflect_pipeline_parameters.device = renderer.device;
	reflect_pipeline_parameters.glfw_window = renderer.window;
	reflect_pipeline_parameters.num_textures = max_lights;
	reflect_pipeline_parameters.num_uniform_buffers = 3;
	reflect_pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
	reflect_pipeline_parameters.pipeline_barriers = shadow_pipeline_barriers;
	reflect_pipeline_parameters.render_pass = reflection_map_render_pass;
	reflect_pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_reflect_map.spv"], renderer.data.shaders["Resources/frag_yellow_reflect.spv"] };
	reflect_pipeline_parameters.swap_chain = renderer.swap_chain;
	reflect_pipeline_parameters.viewport_width = 128;
	reflect_pipeline_parameters.viewport_height = 128;
	reflect_pipeline_parameters.viewport_offset_x = 0;
	reflect_pipeline_parameters.viewport_offset_y = 0;
	reflect_pipeline_parameters.subpass = 0;
	reflect_pipeline_parameters.samples = VK_SAMPLE_COUNT_1_BIT;
	reflect_pipeline_parameters.pipeline_flags = PIPELINE_ORDER_CLOCKWISE;

	create_pipeline(pipeline_yellow_reflect, reflect_pipeline_parameters);
	reflection_map_pipelines.push_back({ "REFLECT_YELLOW", pipeline_yellow_reflect });

	reflect_pipeline_parameters.pipeline_barriers = {};
	reflect_pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_reflect_map.spv"], renderer.data.shaders["Resources/frag_red_reflect.spv"] };
	reflect_pipeline_parameters.access_stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
	reflect_pipeline_parameters.num_uniform_buffers += 1;

	create_pipeline(pipeline_red_reflect, reflect_pipeline_parameters);
	reflection_map_pipelines.push_back({ "REFLECT_RED", pipeline_red_reflect });

	// Create pipelines for box internals
	std::vector<std::pair<std::string, VulkanPipeline>> box_internals_pipelines = {};
	VulkanPipeline box_internals_pipeline = {};
	VulkanPipelineParameters box_internals_pipeline_parameters = {};
	box_internals_pipeline_parameters.attribute_descriptions = attribute_descriptions;
	box_internals_pipeline_parameters.binding_descriptions = binding_descriptions;
	box_internals_pipeline_parameters.device = renderer.device;
	box_internals_pipeline_parameters.glfw_window = renderer.window;
	box_internals_pipeline_parameters.num_textures = 0;
	box_internals_pipeline_parameters.num_uniform_buffers = 2;
	box_internals_pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
	box_internals_pipeline_parameters.pipeline_barriers = {};
	box_internals_pipeline_parameters.render_pass = box_internals_render_pass;
	box_internals_pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_box_internals.spv"], renderer.data.shaders["Resources/frag_box_internals.spv"] };
	box_internals_pipeline_parameters.swap_chain = renderer.swap_chain;
	box_internals_pipeline_parameters.viewport_width = 64;
	box_internals_pipeline_parameters.viewport_height = 64;
	box_internals_pipeline_parameters.viewport_offset_x = 0;
	box_internals_pipeline_parameters.viewport_offset_y = 0;
	box_internals_pipeline_parameters.subpass = 0;
	box_internals_pipeline_parameters.samples = VK_SAMPLE_COUNT_1_BIT;

	create_pipeline(box_internals_pipeline, box_internals_pipeline_parameters);
	box_internals_pipelines.push_back({ "BOX_INTERNALS", box_internals_pipeline });

	// Create render pass managers
	std::vector<VkClearValue> clear_values(4);
	clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clear_values[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clear_values[2].depthStencil = { 1.0f, 0 };
	clear_values[3].depthStencil = { 1.0f, 0 };

	RenderPassManager render_pass_manager = {};
	RenderPassManagerParameters render_pass_manager_parameters = {};
	render_pass_manager_parameters.pass = render_pass;
	render_pass_manager_parameters.pass_pipelines = pipelines;
	render_pass_manager_parameters.clear_values = clear_values;

	RenderPassManager shadow_map_render_pass_manager = {};
	RenderPassManagerParameters shadow_map_render_pass_manager_parameters = {};
	shadow_map_render_pass_manager_parameters.pass = shadow_map_render_pass;
	shadow_map_render_pass_manager_parameters.pass_pipelines = shadow_map_pipelines;
	shadow_map_render_pass_manager_parameters.clear_values = { { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f }, { 1.0f } };

	VulkanMipmapGenerationParameters reflection_map_mipmap_parameters = {};
	reflection_map_mipmap_parameters.src_image = renderer.data.textures["REFLECTION_MAP_ATTACHMENT"];
	reflection_map_mipmap_parameters.dst_image = renderer.data.textures["REFLECTION_MAP_FINAL"];

	RenderPassManager reflection_map_render_pass_manager = {};
	RenderPassManagerParameters reflection_map_render_pass_manager_parameters = {};
	reflection_map_render_pass_manager_parameters.pass = reflection_map_render_pass;
	reflection_map_render_pass_manager_parameters.pass_pipelines = reflection_map_pipelines;
	reflection_map_render_pass_manager_parameters.clear_values = { {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f}} };
	reflection_map_render_pass_manager_parameters.mip_map = true;
	reflection_map_render_pass_manager_parameters.mip_parameters = reflection_map_mipmap_parameters;

	VulkanMipmapGenerationParameters box_internals_mipmap_parameters = {};
	box_internals_mipmap_parameters.src_image = renderer.data.textures["BOX_INTERNALS_ATTACHMENT"];
	box_internals_mipmap_parameters.dst_image = renderer.data.textures["BOX_INTERNALS_FINAL"];

	RenderPassManager box_internals_render_pass_manager = {};
	RenderPassManagerParameters box_internals_render_pass_manager_parameters = {};
	box_internals_render_pass_manager_parameters.pass = box_internals_render_pass;
	box_internals_render_pass_manager_parameters.pass_pipelines = box_internals_pipelines;
	box_internals_render_pass_manager_parameters.clear_values = { {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f}} };
	box_internals_render_pass_manager_parameters.mip_map = true;
	box_internals_render_pass_manager_parameters.mip_parameters = box_internals_mipmap_parameters;

	create_render_pass_manager(render_pass_manager, render_pass_manager_parameters);
	create_render_pass_manager(shadow_map_render_pass_manager, shadow_map_render_pass_manager_parameters);
	create_render_pass_manager(reflection_map_render_pass_manager, reflection_map_render_pass_manager_parameters);
	create_render_pass_manager(box_internals_render_pass_manager, box_internals_render_pass_manager_parameters);
	renderer.render_passes = { shadow_map_render_pass_manager, reflection_map_render_pass_manager, box_internals_render_pass_manager, render_pass_manager };

	// Create materials
	Material mat_pause_screen = {};
	mat_pause_screen.models = { &renderer.data.models["SQUARE"] };
	mat_pause_screen.pipelines = { "standard_pause" };
	mat_pause_screen.textures = {};
	mat_pause_screen.use_lights = LIGHT_USAGE_NONE;
	mat_pause_screen.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_pause_screen.pipelines[0]] };
	mat_pause_screen.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_pause_screen.pipelines[0]] };
	mat_pause_screen.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_pause_screen.pipelines[0]] };

	Material mat_death_screen = {};
	mat_death_screen.models = { &renderer.data.models["SQUARE"] };
	mat_death_screen.pipelines = { "standard_death" };
	mat_death_screen.textures = {};
	mat_death_screen.use_lights = LIGHT_USAGE_NONE;
	mat_death_screen.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_death_screen.pipelines[0]] };
	mat_death_screen.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_death_screen.pipelines[0]] };
	mat_death_screen.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_death_screen.pipelines[0]] };

	Material mat_red_square = {};
	mat_red_square.models = { &renderer.data.models["SQUARE"] };
	mat_red_square.pipelines = { "standard_red" };
	mat_red_square.textures = {};
	mat_red_square.use_lights = LIGHT_USAGE_ALL;
	mat_red_square.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_red_square.pipelines[0]] };
	mat_red_square.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_red_square.pipelines[0]] };
	mat_red_square.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_red_square.pipelines[0]] };

	{
		std::string pipeline = "REFLECT_RED";
		mat_red_square.models.push_back(&renderer.data.models["SQUARE"]);
		mat_red_square.pipelines.push_back(pipeline);
		mat_red_square.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].resources[pipeline]);
		mat_red_square.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].vertex_buffers[pipeline]);
		mat_red_square.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].index_buffers[pipeline]);
	}

	for (uint32_t i = 0; i < max_lights; i++)
	{
		std::string pipeline = "SHADOW_" + std::to_string(i);
		mat_red_square.models.push_back(&renderer.data.models["SQUARE"]);
		mat_red_square.pipelines.push_back(pipeline);
		mat_red_square.textures.push_back({ renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)] });
		mat_red_square.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].resources[pipeline]);
		mat_red_square.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].vertex_buffers[pipeline]);
		mat_red_square.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].index_buffers[pipeline]);
	}

	Material mat_blue_cube = {};
	mat_blue_cube.models = { &renderer.data.models["CUBE"] };
	mat_blue_cube.pipelines = { "standard_blue" };
	mat_blue_cube.textures = {};
	mat_blue_cube.use_lights = LIGHT_USAGE_ALL;
	mat_blue_cube.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_blue_cube.pipelines[0]] };
	mat_blue_cube.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_blue_cube.pipelines[0]] };
	mat_blue_cube.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_blue_cube.pipelines[0]] };

	{
		std::string pipeline = "BOX_INTERNALS";
		mat_blue_cube.models.push_back(&renderer.data.models["SQUARE"]);
		mat_blue_cube.pipelines.push_back(pipeline);
		mat_blue_cube.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_BOX_INTERNALS].resources[pipeline]);
		mat_blue_cube.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_BOX_INTERNALS].vertex_buffers[pipeline]);
		mat_blue_cube.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_BOX_INTERNALS].index_buffers[pipeline]);
	}

	for (uint32_t i = 0; i < max_lights; i++)
	{
		std::string pipeline = "SHADOW_" + std::to_string(i);
		mat_blue_cube.models.push_back(&renderer.data.models["CUBE"]);
		mat_blue_cube.pipelines.push_back(pipeline);
		mat_blue_cube.textures.push_back({ renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)] });
		mat_blue_cube.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].resources[pipeline]);
		mat_blue_cube.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].vertex_buffers[pipeline]);
		mat_blue_cube.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].index_buffers[pipeline]);
	}

	mat_blue_cube.textures.push_back({ renderer.data.textures["REFLECTION_MAP_FINAL"] });
	mat_blue_cube.textures.push_back({ renderer.data.textures["BOX_INTERNALS_FINAL"] });
	mat_blue_cube.textures.push_back({ renderer.data.textures["Resources/Roughness_Top.jpg"] });
	mat_blue_cube.textures.push_back({ renderer.data.textures["Resources/Roughness_Horiz.jpg"] });
	mat_blue_cube.textures.push_back({ renderer.data.textures["Resources/Roughness_Vert.jpg"] });

	Material mat_yellow_cube = {};
	mat_yellow_cube.models = { &renderer.data.models["CUBE"] };
	mat_yellow_cube.pipelines = { "standard_yellow" };
	mat_yellow_cube.textures = {  };
	mat_yellow_cube.use_lights = LIGHT_USAGE_ALL;
	mat_yellow_cube.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_yellow_cube.pipelines[0]] };
	mat_yellow_cube.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_yellow_cube.pipelines[0]] };
	mat_yellow_cube.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_yellow_cube.pipelines[0]] };

	{
		std::string pipeline = "REFLECT_YELLOW";
		mat_yellow_cube.models.push_back(&renderer.data.models["CUBE"]);
		mat_yellow_cube.pipelines.push_back(pipeline);
		mat_yellow_cube.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].resources[pipeline]);
		mat_yellow_cube.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].vertex_buffers[pipeline]);
		mat_yellow_cube.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_REFLECT].index_buffers[pipeline]);
	}

	for (uint32_t i = 0; i < max_lights; i++)
	{
		std::string pipeline = "SHADOW_" + std::to_string(i);
		mat_yellow_cube.models.push_back(&renderer.data.models["CUBE"]);
		mat_yellow_cube.pipelines.push_back(pipeline);
		mat_yellow_cube.textures.push_back({ renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)] });
		mat_yellow_cube.resources.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].resources[pipeline]);
		mat_yellow_cube.vertex_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].vertex_buffers[pipeline]);
		mat_yellow_cube.index_buffers.push_back(&renderer.render_passes[RENDER_PASS_INDEX_SHADOW].index_buffers[pipeline]);
	}

	Material mat_text = {};
	mat_text.models = { &renderer.data.models["SQUARE_TEX_COORDS"] };
	mat_text.pipelines = { "text" };
	mat_text.textures = { {renderer.data.textures["Resources/ARIAL.png"]} };;
	mat_text.use_lights = LIGHT_USAGE_NONE;
	mat_text.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_text.pipelines[0]] };
	mat_text.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_text.pipelines[0]] };
	mat_text.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_text.pipelines[0]] };

	Material mat_volume = {};
	mat_volume.models = { &renderer.data.models["SQUARE"] };
	mat_volume.pipelines = { "volume" };
	mat_volume.textures = {};
	mat_volume.input_attachments = { renderer.data.textures["RENDER_PASS_ATTACHMENT_COLOR"], renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"] };
	mat_volume.use_lights = LIGHT_USAGE_ALL;
	mat_volume.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_volume.pipelines[0]] };
	mat_volume.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_volume.pipelines[0]] };
	mat_volume.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_volume.pipelines[0]] };

	for (uint32_t i = 0; i < max_lights; i++)
	{
		mat_volume.textures.push_back({ renderer.data.textures["SHADOW_MAP_ATTACHMENT_" + std::to_string(i)] });
	}

	Material mat_darken = {};
	mat_darken.models = { &renderer.data.models["SQUARE"] };
	mat_darken.pipelines = { "darken" };
	mat_darken.textures = {};
	mat_darken.input_attachments = { };
	mat_darken.use_lights = LIGHT_USAGE_NONE;
	mat_darken.resources = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].resources[mat_darken.pipelines[0]] };
	mat_darken.vertex_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].vertex_buffers[mat_darken.pipelines[0]] };
	mat_darken.index_buffers = { &renderer.render_passes[RENDER_PASS_INDEX_DRAW].index_buffers[mat_darken.pipelines[0]] };

	renderer.data.materials = { mat_pause_screen, mat_death_screen, mat_red_square, mat_blue_cube, mat_yellow_cube, mat_text, mat_volume, mat_darken };


	// Recreate instance for volumetric fog
	free_instance(renderer, renderer.volume_instance);

	InstanceParameters volume_instance_parameters = {};
	volume_instance_parameters.light_index = -1;
	volume_instance_parameters.material = MATERiAL_VOLUME;
	volume_instance_parameters.uniform_buffers = { {renderer.volume_buffer} };
	renderer.volume_instance = create_instance(renderer, volume_instance_parameters);
}
