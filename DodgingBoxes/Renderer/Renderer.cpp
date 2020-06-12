#include "Renderer.h"

#include <VulkanLayer/Command.h>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <algorithm>

const bool enable_validation_layers = true;

void create_renderer(Renderer &renderer, RendererParameters &parameters)
{
	renderer.instances = {};
	renderer.max_frames = parameters.max_frames;

	// Copy window
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
	color_attachment_parameters.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	color_attachment_parameters.samples = renderer.device.max_sample_count;

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
	depth_attachment_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depth_attachment_parameters.samples = renderer.device.max_sample_count;

	create_texture(depth_attachment, depth_attachment_parameters);

	textures["RENDER_PASS_ATTACHMENT_DEPTH"] = depth_attachment;

	// Create models
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
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
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

	// Description for swap chain image attachment
	VulkanRenderPassAttachment resolve_attachment_description = {};
	resolve_attachment_description.attachment_format = renderer.swap_chain.swap_chain_format;
	resolve_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	resolve_attachment_description.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	resolve_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	// Description for color attachment (Before downsampling)
	VulkanRenderPassAttachment color_attachment_description = {};
	color_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_COLOR"];
	color_attachment_description.attachment_format = color_attachment_description.attachment.format;
	color_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	color_attachment_description.samples = renderer.device.max_sample_count;

	// Description for depth attachment
	VulkanRenderPassAttachment depth_attachment_description = {};
	depth_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"];
	depth_attachment_description.attachment_format = depth_attachment_description.attachment.format;
	depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment_description.samples = renderer.device.max_sample_count;

	// Dependency between external commands and opaque subpass
	VkSubpassDependency opaque_dependency = {};
	opaque_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	opaque_dependency.dstSubpass = 0;
	opaque_dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	opaque_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	opaque_dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	opaque_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	opaque_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Dependency between opaque subpass and transparent subpass
	VkSubpassDependency transparent_opaque_dependency = {};
	transparent_opaque_dependency.srcSubpass = 0;
	transparent_opaque_dependency.dstSubpass = 1;
	transparent_opaque_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	transparent_opaque_dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	transparent_opaque_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	transparent_opaque_dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	transparent_opaque_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Dependency between transparent subpass and external commands
	VkSubpassDependency transparent_external_dependency = {};
	transparent_external_dependency.srcSubpass = 0;
	transparent_external_dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	transparent_external_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	transparent_external_dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	transparent_external_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	transparent_external_dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	transparent_external_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Description of opaque subpass
	VulkanRenderPassSubpassDescription subpass_opaque_description = {};
	subpass_opaque_description.color_attachments = { 1 };
	subpass_opaque_description.depth_attachment = 2;
	subpass_opaque_description.use_depth = true;
	subpass_opaque_description.dependencies = { opaque_dependency };

	// Description of transparent subpass
	VulkanRenderPassSubpassDescription subpass_transparent_description = {};
	subpass_transparent_description.color_attachments = { 1 };
	subpass_transparent_description.resolve_attachments = { 0 };
	subpass_transparent_description.depth_attachment = { 2 };
	subpass_transparent_description.use_depth = true;
	subpass_transparent_description.dependencies = { transparent_opaque_dependency, transparent_external_dependency };

	VulkanRenderPassSubpasses subpasses = {};
	subpasses.attachments = { resolve_attachment_description, color_attachment_description, depth_attachment_description };
	subpasses.subpass_descriptions = { subpass_opaque_description, subpass_transparent_description };

	render_pass_parameters.subpasses = subpasses;

	create_render_pass(render_pass, render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters command_buffer_parameters = {};
	command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(render_pass, command_buffer_parameters);

	// Create pipelines
	int w, h;

	glfwGetFramebufferSize(renderer.window, &w, &h);

	std::unordered_map<std::string, VulkanPipeline> pipelines;
	VulkanPipeline pipeline_green = {};
	VulkanPipeline pipeline_red = {};
	VulkanPipeline pipeline_blue = {};
	VulkanPipeline pipeline_yellow = {};
	VulkanPipeline pipeline_text = {};
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
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard.spv"], renderer.data.shaders["Resources/frag_green.spv"] };
	pipeline_parameters.swap_chain = renderer.swap_chain;
	pipeline_parameters.viewport_width = std::min(w, h);
	pipeline_parameters.viewport_height = std::min(w, h);
	pipeline_parameters.viewport_offset_x = (w - pipeline_parameters.viewport_width) / 2;
	pipeline_parameters.viewport_offset_y = (h - pipeline_parameters.viewport_height) / 2;
	pipeline_parameters.subpass = 0;

	create_pipeline(pipeline_green, pipeline_parameters);
	pipelines["standard_green"] = pipeline_green;

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard.spv"], renderer.data.shaders["Resources/frag_red.spv"] };

	create_pipeline(pipeline_red, pipeline_parameters);
	pipelines["standard_red"] = pipeline_red;

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_blue.spv"] };

	create_pipeline(pipeline_blue, pipeline_parameters);
	pipelines["standard_blue"] = pipeline_blue;

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_yellow.spv"] };

	create_pipeline(pipeline_yellow, pipeline_parameters);
	pipelines["standard_yellow"] = pipeline_yellow;

	pipeline_parameters.attribute_descriptions = attribute_descriptions_tex_coords;
	pipeline_parameters.binding_descriptions = binding_descriptions_tex_coords;
	pipeline_parameters.num_textures = 1;
	pipeline_parameters.num_uniform_buffers = 1;
	pipeline_parameters.subpass = 1;
	pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT };
	pipeline_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_BLEND_ENABLE | PIPELINE_BACKFACE_CULL_DISABLE | PIPELINE_DEPTH_TEST_DISABLE);
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_text.spv"], renderer.data.shaders["Resources/frag_text.spv"] };

	create_pipeline(pipeline_text, pipeline_parameters);
	pipelines["text"] = pipeline_text;

	// Create resources
	// None yet...

	// Create render pass manager
	RenderPassManager render_pass_manager = {};
	RenderPassManagerParameters render_pass_manager_parameters = {};
	render_pass_manager_parameters.pass = render_pass;
	render_pass_manager_parameters.pass_pipelines = pipelines;

	create_render_pass_manager(render_pass_manager, render_pass_manager_parameters);
	renderer.render_passes = { render_pass_manager };

	// Create materials
	Material mat_green_cube = {};
	mat_green_cube.models = { &renderer.data.models["CUBE"] };
	mat_green_cube.pipelines = { "standard_green" };
	mat_green_cube.textures = {};
	mat_green_cube.use_lights = LIGHT_USAGE_ALL;
	mat_green_cube.resources = { &renderer.render_passes[0].resources[mat_green_cube.pipelines[0]] };
	mat_green_cube.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_green_cube.pipelines[0]] };
	mat_green_cube.index_buffers = { &renderer.render_passes[0].index_buffers[mat_green_cube.pipelines[0]] };

	Material mat_red_square = {};
	mat_red_square.models = { &renderer.data.models["SQUARE"] };
	mat_red_square.pipelines = { "standard_red" };
	mat_red_square.textures = {};
	mat_red_square.use_lights = LIGHT_USAGE_ALL;
	mat_red_square.resources = { &renderer.render_passes[0].resources[mat_red_square.pipelines[0]] };
	mat_red_square.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_red_square.pipelines[0]] };
	mat_red_square.index_buffers = { &renderer.render_passes[0].index_buffers[mat_red_square.pipelines[0]] };

	Material mat_blue_cube = {};
	mat_blue_cube.models = { &renderer.data.models["CUBE"] };
	mat_blue_cube.pipelines = { "standard_blue" };
	mat_blue_cube.textures = {};
	mat_blue_cube.use_lights = LIGHT_USAGE_ALL;
	mat_blue_cube.resources = { &renderer.render_passes[0].resources[mat_blue_cube.pipelines[0]] };
	mat_blue_cube.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_blue_cube.pipelines[0]] };
	mat_blue_cube.index_buffers = { &renderer.render_passes[0].index_buffers[mat_blue_cube.pipelines[0]] };

	Material mat_yellow_cube = {};
	mat_yellow_cube.models = { &renderer.data.models["CUBE"] };
	mat_yellow_cube.pipelines = { "standard_yellow" };
	mat_yellow_cube.textures = {};
	mat_yellow_cube.use_lights = LIGHT_USAGE_ALL;
	mat_yellow_cube.resources = { &renderer.render_passes[0].resources[mat_yellow_cube.pipelines[0]] };
	mat_yellow_cube.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_yellow_cube.pipelines[0]] };
	mat_yellow_cube.index_buffers = { &renderer.render_passes[0].index_buffers[mat_yellow_cube.pipelines[0]] };

	Material mat_text = {};
	mat_text.models = { &renderer.data.models["SQUARE_TEX_COORDS"] };
	mat_text.pipelines = { "text" };
	mat_text.textures = { {renderer.data.textures["Resources/ARIAL.png"]} };;
	mat_text.use_lights = LIGHT_USAGE_NONE;
	mat_text.resources = { &renderer.render_passes[0].resources[mat_text.pipelines[0]] };
	mat_text.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_text.pipelines[0]] };
	mat_text.index_buffers = { &renderer.render_passes[0].index_buffers[mat_text.pipelines[0]] };


	renderer.data.materials = { mat_green_cube, mat_red_square, mat_blue_cube, mat_yellow_cube, mat_text };

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

void draw(Renderer &renderer, DrawParameters &parameters)
{
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
		std::vector<std::vector<VulkanBuffer>> index_buffers;
		std::vector<std::vector<VulkanBuffer>> vertex_buffers;
		std::vector<std::vector<VulkanResource>> resources;
		std::vector<std::vector<VulkanPipeline>> pipelines(render_pass.pass.total_subpasses);

		// Information grouped according to subpass
		std::vector<VulkanSubpassCommandBufferRecordDetails> subpasses(render_pass.pass.total_subpasses);
		for (auto &pipeline : render_pass.pass_pipelines)
		{
			subpasses[pipeline.second.subpass].vertex_buffers.emplace_back(render_pass.vertex_buffers[pipeline.first]);
			subpasses[pipeline.second.subpass].index_buffers.emplace_back(render_pass.index_buffers[pipeline.first]);
			subpasses[pipeline.second.subpass].resources.emplace_back(render_pass.resources[pipeline.first]);
			subpasses[pipeline.second.subpass].pipelines.emplace_back(pipeline.second);

			render_pass.vertex_buffers[pipeline.first].clear();
			render_pass.index_buffers[pipeline.first].clear();
			render_pass.resources[pipeline.first].clear();
		}

		VulkanRenderPassCommandBufferRecordParameters record_parameters;
		record_parameters.device = renderer.device;
		record_parameters.subpasses = subpasses;
		record_parameters.swap_chain = renderer.swap_chain;
		record_parameters.framebuffer_index = renderer.image_index;

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

	submit_info.commandBufferCount = command_buffers.size();
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

	// TODO: this might be a little too slow
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

	std::vector<VulkanPipeline> chosen_pipelines = {};
	uint32_t render_pass_index = 0;
	for (uint32_t k = 0; k < mat.pipelines.size(); k++)
	{
		for (uint32_t i = 0; i < renderer.render_passes.size(); i++)
		{
			auto render_pass = renderer.render_passes[i];
			for (auto pipeline : render_pass.pass_pipelines)
			{
				if (pipeline.first == mat.pipelines[k])
				{

					chosen_pipelines.push_back(pipeline.second);
					render_pass_index = i;
					break;
				}
			}

			if (chosen_pipelines.size() > k)
			{
				break;
			}
		}
		for (auto &chosen_pipeline : chosen_pipelines)
		{
			if (chosen_pipeline.device == VK_NULL_HANDLE)
			{
				throw std::runtime_error("Could not find pipeline!");
			}
		}
	}

	// Generate name
	std::string name = std::to_string(parameters.material);

	for (auto uniform_buffer : parameters.uniform_buffers)
	{
		name += "_" + uniform_buffer;
	}

	// Make sure resource of this description doesn't already exist
	for (auto render_pass : renderer.render_passes)
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
	resource_parameters.textures = mat.textures;

	// NOTE: chosen_pipelines should be the same size as parameters.uniform_buffers
	for (uint32_t i = 0; i < parameters.uniform_buffers.size(); i++)
	{
		resource_parameters.pipeline = chosen_pipelines[i];
		// If lights are not being used
		if (mat.use_lights == LIGHT_USAGE_NONE)
		{
			// Submit just the one uniform buffer
			resource_parameters.uniform_buffers = { renderer.data.uniform_buffers[parameters.uniform_buffers[i]].buffers };
		}
		else if (mat.use_lights == LIGHT_USAGE_ALL)
		{
			// Otherwise also use the uniform buffer with lighting information
			resource_parameters.uniform_buffers = { renderer.data.uniform_buffers[parameters.uniform_buffers[i]].buffers, renderer.data.uniform_buffers[renderer.light_buffers].buffers };
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
	Instance instance = renderer.instances[parameters.instance_name];
	Material material = renderer.data.materials[instance.material];

	// Note: instance.resources must be the same size as material.resources
	for (uint32_t i = 0; i < material.resources.size(); i++)
	{
		material.resources[i]->emplace_back(instance.resources[i]);
		material.vertex_buffers[i]->emplace_back(material.models[i]->first);
		material.index_buffers[i]->emplace_back(material.models[i]->second);
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
	// TODO: There's probably a better way to do this
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

	VulkanTexture color_attachment = {};
	VulkanTextureParameters color_attachment_parameters = {};
	color_attachment_parameters.device = renderer.device;
	color_attachment_parameters.command_pool = renderer.device.command_pool;
	color_attachment_parameters.memory_manager = &renderer.memory_manager;
	color_attachment_parameters.format = renderer.swap_chain.swap_chain_format;
	color_attachment_parameters.height = renderer.swap_chain.swap_chain_extent.height;
	color_attachment_parameters.width = renderer.swap_chain.swap_chain_extent.width;
	color_attachment_parameters.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	color_attachment_parameters.samples = renderer.device.max_sample_count;

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
	depth_attachment_parameters.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depth_attachment_parameters.samples = renderer.device.max_sample_count;

	create_texture(depth_attachment, depth_attachment_parameters);

	renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"] = depth_attachment;

	// Create render passes
	VulkanRenderPass render_pass = {};
	VulkanRenderPassParameters render_pass_parameters = {};
	render_pass_parameters.device = renderer.device;
	render_pass_parameters.glfw_window = renderer.window;
	render_pass_parameters.memory_manager = &(renderer.memory_manager);
	render_pass_parameters.swap_chain = renderer.swap_chain;

	VulkanRenderPassAttachment resolve_attachment_description = {};
	resolve_attachment_description.attachment_format = renderer.swap_chain.swap_chain_format;
	resolve_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	resolve_attachment_description.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	resolve_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;

	VulkanRenderPassAttachment color_attachment_description = {};
	color_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_COLOR"];
	color_attachment_description.attachment_format = color_attachment_description.attachment.format;
	color_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	color_attachment_description.samples = renderer.device.max_sample_count;

	VulkanRenderPassAttachment depth_attachment_description = {};
	depth_attachment_description.attachment = renderer.data.textures["RENDER_PASS_ATTACHMENT_DEPTH"];
	depth_attachment_description.attachment_format = depth_attachment_description.attachment.format;
	depth_attachment_description.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment_description.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment_description.samples = renderer.device.max_sample_count;

	VkSubpassDependency opaque_dependency = {};
	opaque_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	opaque_dependency.dstSubpass = 0;
	opaque_dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	opaque_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	opaque_dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	opaque_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	opaque_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency transparent_opaque_dependency = {};
	transparent_opaque_dependency.srcSubpass = 0;
	transparent_opaque_dependency.dstSubpass = 1;
	transparent_opaque_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	transparent_opaque_dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	transparent_opaque_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	transparent_opaque_dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	transparent_opaque_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency transparent_external_dependency = {};
	transparent_external_dependency.srcSubpass = 0;
	transparent_external_dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	transparent_external_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	transparent_external_dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	transparent_external_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	transparent_external_dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	transparent_external_dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VulkanRenderPassSubpassDescription subpass_opaque_description = {};
	subpass_opaque_description.color_attachments = { 1 };
	subpass_opaque_description.depth_attachment = 2;
	subpass_opaque_description.use_depth = true;
	subpass_opaque_description.dependencies = { opaque_dependency };

	VulkanRenderPassSubpassDescription subpass_transparent_description = {};
	subpass_transparent_description.color_attachments = { 1 };
	subpass_transparent_description.resolve_attachments = { 0 };
	subpass_transparent_description.depth_attachment = { 2 };
	subpass_transparent_description.use_depth = true;
	subpass_transparent_description.dependencies = { transparent_opaque_dependency, transparent_external_dependency };

	VulkanRenderPassSubpasses subpasses = {};
	subpasses.attachments = { resolve_attachment_description, color_attachment_description, depth_attachment_description };
	subpasses.subpass_descriptions = { subpass_opaque_description, subpass_transparent_description };

	render_pass_parameters.subpasses = subpasses;

	create_render_pass(render_pass, render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters command_buffer_parameters = {};
	command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(render_pass, command_buffer_parameters);

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

	// Create pipelines
	int w, h;

	glfwGetFramebufferSize(renderer.window, &w, &h);

	std::unordered_map<std::string, VulkanPipeline> pipelines;
	VulkanPipeline pipeline_green = {};
	VulkanPipeline pipeline_red = {};
	VulkanPipeline pipeline_blue = {};
	VulkanPipeline pipeline_yellow = {};
	VulkanPipeline pipeline_text = {};
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
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard.spv"], renderer.data.shaders["Resources/frag_green.spv"] };
	pipeline_parameters.swap_chain = renderer.swap_chain;
	pipeline_parameters.viewport_width = std::min(w, h);
	pipeline_parameters.viewport_height = std::min(w, h);
	pipeline_parameters.viewport_offset_x = (w - pipeline_parameters.viewport_width) / 2;
	pipeline_parameters.viewport_offset_y = (h - pipeline_parameters.viewport_height) / 2;
	pipeline_parameters.subpass = 0;

	create_pipeline(pipeline_green, pipeline_parameters);
	pipelines["standard_green"] = pipeline_green;

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard.spv"], renderer.data.shaders["Resources/frag_red.spv"] };

	create_pipeline(pipeline_red, pipeline_parameters);
	pipelines["standard_red"] = pipeline_red;

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_blue.spv"] };

	create_pipeline(pipeline_blue, pipeline_parameters);
	pipelines["standard_blue"] = pipeline_blue;

	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_standard_light_index.spv"], renderer.data.shaders["Resources/frag_yellow.spv"] };

	create_pipeline(pipeline_yellow, pipeline_parameters);
	pipelines["standard_yellow"] = pipeline_yellow;

	pipeline_parameters.attribute_descriptions = attribute_descriptions_tex_coords;
	pipeline_parameters.binding_descriptions = binding_descriptions_tex_coords;
	pipeline_parameters.num_textures = 1;
	pipeline_parameters.num_uniform_buffers = 1;
	pipeline_parameters.subpass = 1;
	pipeline_parameters.access_stages = { VK_SHADER_STAGE_VERTEX_BIT };
	pipeline_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_BLEND_ENABLE | PIPELINE_BACKFACE_CULL_DISABLE | PIPELINE_DEPTH_TEST_DISABLE);
	pipeline_parameters.shaders = { renderer.data.shaders["Resources/vert_text.spv"], renderer.data.shaders["Resources/frag_text.spv"] };

	create_pipeline(pipeline_text, pipeline_parameters);
	pipelines["text"] = pipeline_text;

	// Create render pass manager
	RenderPassManager render_pass_manager = {};
	RenderPassManagerParameters render_pass_manager_parameters = {};
	render_pass_manager_parameters.pass = render_pass;
	render_pass_manager_parameters.pass_pipelines = pipelines;

	create_render_pass_manager(render_pass_manager, render_pass_manager_parameters);
	renderer.render_passes = { render_pass_manager };

	// Create materials
	Material mat_green_cube = {};
	mat_green_cube.models = { &renderer.data.models["CUBE"] };
	mat_green_cube.pipelines = { "standard_green" };
	mat_green_cube.textures = {};
	mat_green_cube.use_lights = LIGHT_USAGE_ALL;
	mat_green_cube.resources = { &renderer.render_passes[0].resources[mat_green_cube.pipelines[0]] };
	mat_green_cube.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_green_cube.pipelines[0]] };
	mat_green_cube.index_buffers = { &renderer.render_passes[0].index_buffers[mat_green_cube.pipelines[0]] };

	Material mat_red_square = {};
	mat_red_square.models = { &renderer.data.models["SQUARE"] };
	mat_red_square.pipelines = { "standard_red" };
	mat_red_square.textures = {};
	mat_red_square.use_lights = LIGHT_USAGE_ALL;
	mat_red_square.resources = { &renderer.render_passes[0].resources[mat_red_square.pipelines[0]] };
	mat_red_square.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_red_square.pipelines[0]] };
	mat_red_square.index_buffers = { &renderer.render_passes[0].index_buffers[mat_red_square.pipelines[0]] };

	Material mat_blue_cube = {};
	mat_blue_cube.models = { &renderer.data.models["CUBE"] };
	mat_blue_cube.pipelines = { "standard_blue" };
	mat_blue_cube.textures = {};
	mat_blue_cube.use_lights = LIGHT_USAGE_ALL;
	mat_blue_cube.resources = { &renderer.render_passes[0].resources[mat_blue_cube.pipelines[0]] };
	mat_blue_cube.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_blue_cube.pipelines[0]] };
	mat_blue_cube.index_buffers = { &renderer.render_passes[0].index_buffers[mat_blue_cube.pipelines[0]] };

	Material mat_yellow_cube = {};
	mat_yellow_cube.models = { &renderer.data.models["CUBE"] };
	mat_yellow_cube.pipelines = { "standard_yellow" };
	mat_yellow_cube.textures = {};
	mat_yellow_cube.use_lights = LIGHT_USAGE_ALL;
	mat_yellow_cube.resources = { &renderer.render_passes[0].resources[mat_yellow_cube.pipelines[0]] };
	mat_yellow_cube.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_yellow_cube.pipelines[0]] };
	mat_yellow_cube.index_buffers = { &renderer.render_passes[0].index_buffers[mat_yellow_cube.pipelines[0]] };

	Material mat_text = {};
	mat_text.models = { &renderer.data.models["SQUARE_TEX_COORDS"] };
	mat_text.pipelines = { "text" };
	mat_text.textures = { {renderer.data.textures["Resources/ARIAL.png"]} };;
	mat_text.use_lights = LIGHT_USAGE_NONE;
	mat_text.resources = { &renderer.render_passes[0].resources[mat_text.pipelines[0]] };
	mat_text.vertex_buffers = { &renderer.render_passes[0].vertex_buffers[mat_text.pipelines[0]] };
	mat_text.index_buffers = { &renderer.render_passes[0].index_buffers[mat_text.pipelines[0]] };


	renderer.data.materials = { mat_green_cube, mat_red_square, mat_blue_cube, mat_yellow_cube, mat_text };
}