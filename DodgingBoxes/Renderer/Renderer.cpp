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
		if (shader_file.substr(0, 4) == "vert")
		{
			shader_parameters.type = 'v';
		}
		else if (shader_file.substr(0, 4) == "frag")
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

	// Create models
	std::unordered_map<std::string, std::pair<VulkanBuffer, VulkanBuffer>> models = {};
	VkVertexInputBindingDescription binding_description = {};
	binding_description.binding = 0;
	binding_description.stride = sizeof(glm::vec3);
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputBindingDescription> binding_descriptions = { binding_description };

	VkVertexInputAttributeDescription attribute_description = {};

	attribute_description.binding = 0;
	attribute_description.location = 0;
	attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description.offset = 0;
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions = { attribute_description };

	std::vector<glm::vec3> square_vertices_data = {
		{-0.5f, -0.5f, 0.5f},
		{0.5f, -0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f},
		{-0.5f, 0.5f, 0.5f}
	};

	std::vector<uint32_t> square_indices_data = {
		0, 1, 2, 2, 3, 0
	};

	VulkanBufferParameters square_vertices_parameters = {};
	square_vertices_parameters.data = (void *)square_vertices_data.data();
	square_vertices_parameters.device = renderer.device;
	square_vertices_parameters.memory_manager = &(renderer.memory_manager);
	square_vertices_parameters.range = sizeof(glm::vec3);
	square_vertices_parameters.size = sizeof(glm::vec3) * square_vertices_data.size();
	square_vertices_parameters.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	square_vertices_parameters.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VulkanBuffer square_vertices_buffer = {};
	create_buffer(square_vertices_buffer, square_vertices_parameters);

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

	std::vector<glm::vec3> cube_vertices_data = {
		{-0.5f, -0.5f, -0.5f},
		{0.5f, -0.5f, -0.5f},
		{0.5f, 0.5f, -0.5f},
		{-0.5f, 0.5f, -0.5f},

		{-0.5f, -0.5f, 0.5f},
		{0.5f, -0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f},
		{-0.5f, 0.5f, 0.5f}
	};

	std::vector<uint32_t> cube_indices_data = {
		0, 2, 1, 3, 2, 0,
		0, 4, 3, 3, 4, 7,
		1, 2, 5, 5, 2, 6,
		6, 7, 5, 5, 7, 4,
		5, 4, 1, 1, 4, 0,
		2, 3, 6, 6, 3, 7
	};

	VulkanBufferParameters cube_vertices_parameters = {};
	cube_vertices_parameters.data = (void *)cube_vertices_data.data();
	cube_vertices_parameters.device = renderer.device;
	cube_vertices_parameters.memory_manager = &(renderer.memory_manager);
	cube_vertices_parameters.range = sizeof(glm::vec3);
	cube_vertices_parameters.size = sizeof(glm::vec3) * cube_vertices_data.size();
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
	VulkanPipelineParameters pipeline_parameters = {};
	pipeline_parameters.attribute_descriptions = attribute_descriptions;
	pipeline_parameters.binding_descriptions = binding_descriptions;
	pipeline_parameters.device = renderer.device;
	pipeline_parameters.glfw_window = renderer.window;
	pipeline_parameters.num_textures = 0;
	pipeline_parameters.pipeline_barriers = {};
	pipeline_parameters.render_pass = render_pass;
	pipeline_parameters.shaders = { renderer.data.shaders["vert_standard.spv"], renderer.data.shaders["frag_green.spv"] };
	pipeline_parameters.swap_chain = renderer.swap_chain;
	pipeline_parameters.viewport_width = std::min(w, h);
	pipeline_parameters.viewport_height = std::min(w, h);
	pipeline_parameters.viewport_offset_x = (w - pipeline_parameters.viewport_width) / 2;
	pipeline_parameters.viewport_offset_y = (h - pipeline_parameters.viewport_height) / 2;

	create_pipeline(pipeline_green, pipeline_parameters);
	pipelines["standard_green"] = pipeline_green;

	pipeline_parameters.shaders = { renderer.data.shaders["vert_standard.spv"], renderer.data.shaders["frag_red.spv"] };

	create_pipeline(pipeline_red, pipeline_parameters);
	pipelines["standard_red"] = pipeline_red;

	pipeline_parameters.shaders = { renderer.data.shaders["vert_standard.spv"], renderer.data.shaders["frag_blue.spv"] };

	create_pipeline(pipeline_blue, pipeline_parameters);
	pipelines["standard_blue"] = pipeline_blue;

	pipeline_parameters.shaders = { renderer.data.shaders["vert_standard.spv"], renderer.data.shaders["frag_yellow.spv"] };
	pipeline_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_BLEND_ENABLE | PIPELINE_BACKFACE_CULL_DISABLE);

	create_pipeline(pipeline_yellow, pipeline_parameters);
	pipelines["standard_yellow"] = pipeline_yellow;

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
	mat_green_cube.model = &renderer.data.models["CUBE"];
	mat_green_cube.pipeline = "standard_green";
	mat_green_cube.textures = {};
	mat_green_cube.resources = &renderer.render_passes[0].resources[mat_green_cube.pipeline];
	mat_green_cube.vertex_buffers = &renderer.render_passes[0].vertex_buffers[mat_green_cube.pipeline];
	mat_green_cube.index_buffers = &renderer.render_passes[0].index_buffers[mat_green_cube.pipeline];

	Material mat_red_square = {};
	mat_red_square.model = &renderer.data.models["SQUARE"];
	mat_red_square.pipeline = "standard_red";
	mat_red_square.textures = {};
	mat_red_square.resources = &renderer.render_passes[0].resources[mat_red_square.pipeline];
	mat_red_square.vertex_buffers = &renderer.render_passes[0].vertex_buffers[mat_red_square.pipeline];
	mat_red_square.index_buffers = &renderer.render_passes[0].index_buffers[mat_red_square.pipeline];

	Material mat_blue_cube = {};
	mat_blue_cube.model = &renderer.data.models["CUBE"];
	mat_blue_cube.pipeline = "standard_blue";
	mat_blue_cube.textures = {};
	mat_blue_cube.resources = &renderer.render_passes[0].resources[mat_blue_cube.pipeline];
	mat_blue_cube.vertex_buffers = &renderer.render_passes[0].vertex_buffers[mat_blue_cube.pipeline];
	mat_blue_cube.index_buffers = &renderer.render_passes[0].index_buffers[mat_blue_cube.pipeline];

	Material mat_yellow_cube = {};
	mat_yellow_cube.model = &renderer.data.models["CUBE"];
	mat_yellow_cube.pipeline = "standard_yellow";
	mat_yellow_cube.textures = {};
	mat_yellow_cube.resources = &renderer.render_passes[0].resources[mat_yellow_cube.pipeline];
	mat_yellow_cube.vertex_buffers = &renderer.render_passes[0].vertex_buffers[mat_yellow_cube.pipeline];
	mat_yellow_cube.index_buffers = &renderer.render_passes[0].index_buffers[mat_yellow_cube.pipeline];


	renderer.data.materials = { mat_green_cube, mat_red_square, mat_blue_cube, mat_yellow_cube };

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
	// Record command buffers
	std::vector<std::pair<VulkanRenderPass*, std::vector<Instance*>>> pass_instances;

	for (auto &render_pass : renderer.render_passes)
	{
		std::vector<std::vector<VulkanBuffer>> index_buffers;
		std::vector<std::vector<VulkanBuffer>> vertex_buffers;
		std::vector<std::vector<VulkanResource>> resources;
		std::vector<VulkanPipeline> pipelines;
		for (auto &pipeline : render_pass.pass_pipelines)
		{
			vertex_buffers.emplace_back(render_pass.vertex_buffers[pipeline.first]);
			index_buffers.emplace_back(render_pass.index_buffers[pipeline.first]);
			resources.emplace_back(render_pass.resources[pipeline.first]);
			pipelines.emplace_back(pipeline.second);

			render_pass.vertex_buffers[pipeline.first].clear();
			render_pass.index_buffers[pipeline.first].clear();
			render_pass.resources[pipeline.first].clear();
		}

		VulkanRenderPassCommandBufferRecordParameters record_parameters;
		record_parameters.device = renderer.device;
		record_parameters.index_buffers = index_buffers;
		record_parameters.pipelines = pipelines;
		record_parameters.resources = resources;
		record_parameters.swap_chain = renderer.swap_chain;
		record_parameters.vertex_buffers = vertex_buffers;
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
		cleanup_resource(instance.second.resource);
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
	uniform_buffer_parameters.range = parameters.size;

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

	VulkanPipeline chosen_pipeline = {};
	uint32_t render_pass_index = 0;
	for (uint32_t i = 0; i < renderer.render_passes.size(); i++)
	{
		auto render_pass = renderer.render_passes[i];
		for (auto pipeline : render_pass.pass_pipelines)
		{
			if (pipeline.first == mat.pipeline)
			{

				chosen_pipeline = pipeline.second;
				render_pass_index = i;
				break;
			}
		}

		if (chosen_pipeline.device != VK_NULL_HANDLE)
		{
			break;
		}
	}
	if (chosen_pipeline.device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Could not find pipeline!");
	}

	// Generate name
	std::string name = std::to_string(render_pass_index) + '_' + std::to_string( parameters.material) + "_" + parameters.uniform_buffer;

	// Make sure resource of this description doesn't already exist
	for (auto render_pass : renderer.render_passes)
	{
		if (render_pass.resources.find(name) != render_pass.resources.end())
		{
			throw std::runtime_error("Tried to create resource which already exists!");
		}
	}

	// Fill out structure
	VulkanResource resource = {};
	VulkanResourceParameters resource_parameters = {};
	resource_parameters.device = renderer.device;
	resource_parameters.swap_chain = renderer.swap_chain;
	resource_parameters.pipeline = chosen_pipeline;
	resource_parameters.textures = mat.textures;
	resource_parameters.uniform_buffers = renderer.data.uniform_buffers[parameters.uniform_buffer].buffers;

	create_resource(resource, resource_parameters);

	Instance instance = {};
	instance.resource = resource;
	instance.material = parameters.material;

	renderer.instances[name] = instance;
	return name;
}

void submit_instance(Renderer &renderer, InstanceSubmitParameters &parameters)
{
	Instance instance = renderer.instances[parameters.instance_name];
	Material material = renderer.data.materials[instance.material];

	material.resources->emplace_back(instance.resource);
	material.vertex_buffers->emplace_back(material.model->first);
	material.index_buffers->emplace_back(material.model->second);
}

void free_instance(Renderer &renderer, std::string instance_name)
{
	auto &instance = renderer.instances[instance_name];
	cleanup_resource(instance.resource);
	renderer.instances.erase(instance_name);
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
	for (auto &model : data_manager.models)
	{
		cleanup_buffer(model.second.first);
		cleanup_buffer(model.second.second);
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

	// Create render passes
	VulkanRenderPass render_pass = {};
	VulkanRenderPassParameters render_pass_parameters = {};
	render_pass_parameters.device = renderer.device;
	render_pass_parameters.glfw_window = renderer.window;
	render_pass_parameters.memory_manager = &(renderer.memory_manager);
	render_pass_parameters.swap_chain = renderer.swap_chain;

	create_render_pass(render_pass, render_pass_parameters);

	VulkanRenderPassCommandBufferAllocateParameters command_buffer_parameters = {};
	command_buffer_parameters.swap_chain = renderer.swap_chain;
	allocate_render_pass_command_buffers(render_pass, command_buffer_parameters);

	// Create attribute and binding description
	VkVertexInputAttributeDescription attribute_description = {};

	attribute_description.binding = 0;
	attribute_description.location = 0;
	attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_description.offset = 0;
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions = { attribute_description };

	VkVertexInputBindingDescription binding_description = {};
	binding_description.binding = 0;
	binding_description.stride = sizeof(glm::vec3);
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputBindingDescription> binding_descriptions = { binding_description };

	// Create pipelines
	int w, h;

	glfwGetFramebufferSize(renderer.window, &w, &h);

	std::unordered_map<std::string, VulkanPipeline> pipelines;
	VulkanPipeline pipeline_green = {};
	VulkanPipeline pipeline_red = {};
	VulkanPipeline pipeline_blue = {};
	VulkanPipeline pipeline_yellow = {};
	VulkanPipelineParameters pipeline_parameters = {};
	pipeline_parameters.attribute_descriptions = attribute_descriptions;
	pipeline_parameters.binding_descriptions = binding_descriptions;
	pipeline_parameters.device = renderer.device;
	pipeline_parameters.glfw_window = renderer.window;
	pipeline_parameters.num_textures = 0;
	pipeline_parameters.pipeline_barriers = {};
	pipeline_parameters.render_pass = render_pass;
	pipeline_parameters.shaders = { renderer.data.shaders["vert_standard.spv"], renderer.data.shaders["frag_green.spv"] };
	pipeline_parameters.swap_chain = renderer.swap_chain;
	pipeline_parameters.viewport_width = std::min(w, h);
	pipeline_parameters.viewport_height = std::min(w, h);
	pipeline_parameters.viewport_offset_x = (w - pipeline_parameters.viewport_width) / 2;
	pipeline_parameters.viewport_offset_y = (h - pipeline_parameters.viewport_height) / 2;

	create_pipeline(pipeline_green, pipeline_parameters);
	pipelines["standard_green"] = pipeline_green;

	pipeline_parameters.shaders = { renderer.data.shaders["vert_standard.spv"], renderer.data.shaders["frag_red.spv"] };

	create_pipeline(pipeline_red, pipeline_parameters);
	pipelines["standard_red"] = pipeline_red;

	pipeline_parameters.shaders = { renderer.data.shaders["vert_standard.spv"], renderer.data.shaders["frag_blue.spv"] };

	create_pipeline(pipeline_blue, pipeline_parameters);
	pipelines["standard_blue"] = pipeline_blue;

	pipeline_parameters.shaders = { renderer.data.shaders["vert_standard.spv"], renderer.data.shaders["frag_yellow.spv"] };
	pipeline_parameters.pipeline_flags = static_cast<PipelineFlags>(PIPELINE_BLEND_ENABLE | PIPELINE_BACKFACE_CULL_DISABLE);

	create_pipeline(pipeline_yellow, pipeline_parameters);
	pipelines["standard_yellow"] = pipeline_yellow;

	// Create render pass manager
	RenderPassManager render_pass_manager = {};
	RenderPassManagerParameters render_pass_manager_parameters = {};
	render_pass_manager_parameters.pass = render_pass;
	render_pass_manager_parameters.pass_pipelines = pipelines;

	create_render_pass_manager(render_pass_manager, render_pass_manager_parameters);
	renderer.render_passes = { render_pass_manager };

	// Create materials
	Material mat_green_cube = {};
	mat_green_cube.model = &renderer.data.models["CUBE"];
	mat_green_cube.pipeline = "standard_green";
	mat_green_cube.textures = {};
	mat_green_cube.resources = &renderer.render_passes[0].resources[mat_green_cube.pipeline];
	mat_green_cube.vertex_buffers = &renderer.render_passes[0].vertex_buffers[mat_green_cube.pipeline];
	mat_green_cube.index_buffers = &renderer.render_passes[0].index_buffers[mat_green_cube.pipeline];

	Material mat_red_square = {};
	mat_red_square.model = &renderer.data.models["SQUARE"];
	mat_red_square.pipeline = "standard_red";
	mat_red_square.textures = {};
	mat_red_square.resources = &renderer.render_passes[0].resources[mat_red_square.pipeline];
	mat_red_square.vertex_buffers = &renderer.render_passes[0].vertex_buffers[mat_red_square.pipeline];
	mat_red_square.index_buffers = &renderer.render_passes[0].index_buffers[mat_red_square.pipeline];

	Material mat_blue_cube = {};
	mat_blue_cube.model = &renderer.data.models["CUBE"];
	mat_blue_cube.pipeline = "standard_blue";
	mat_blue_cube.textures = {};
	mat_blue_cube.resources = &renderer.render_passes[0].resources[mat_blue_cube.pipeline];
	mat_blue_cube.vertex_buffers = &renderer.render_passes[0].vertex_buffers[mat_blue_cube.pipeline];
	mat_blue_cube.index_buffers = &renderer.render_passes[0].index_buffers[mat_blue_cube.pipeline];

	Material mat_yellow_cube = {};
	mat_yellow_cube.model = &renderer.data.models["CUBE"];
	mat_yellow_cube.pipeline = "standard_yellow";
	mat_yellow_cube.textures = {};
	mat_yellow_cube.resources = &renderer.render_passes[0].resources[mat_yellow_cube.pipeline];
	mat_yellow_cube.vertex_buffers = &renderer.render_passes[0].vertex_buffers[mat_yellow_cube.pipeline];
	mat_yellow_cube.index_buffers = &renderer.render_passes[0].index_buffers[mat_yellow_cube.pipeline];


	renderer.data.materials = { mat_green_cube, mat_red_square, mat_blue_cube, mat_yellow_cube };
}