#include "Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <iostream>

const int max_frames = 2;

const int width = 800;
const int height = 600;

const std::vector<std::string> models = {
	
};

const std::vector<std::string> shaders = {
	"vert_standard.spv",
	"frag_green.spv"
};

const std::vector<std::string> textures = {
	
};

struct UB
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

int main()
{
	// Set up glfw and create window
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window = glfwCreateWindow(width, height, "Dodgin' Boxes", nullptr, nullptr);

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> instance_extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	std::vector<const char *> physical_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// Create renderer
	Renderer renderer = {};
	RendererParameters renderer_parameters = {};
	renderer_parameters.instance_extensions = instance_extensions;
	renderer_parameters.physical_extensions = physical_extensions;
	renderer_parameters.model_files = models;
	renderer_parameters.shader_files = shaders;
	renderer_parameters.texture_files = textures;
	renderer_parameters.window = window;
	renderer_parameters.max_frames = max_frames;
	
	create_renderer(renderer, renderer_parameters);

	UniformBufferParameters uniform_parameters = {};
	uniform_parameters.size = sizeof(UB);

	std::string uniform_buffer_name = get_uniform_buffer(renderer, uniform_parameters);
	/*std::string uniform_buffer_name_2 = get_uniform_buffer(renderer, uniform_parameters)*/;

	InstanceParameters instance_parameters = {};
	instance_parameters.model = "CUBE";
	instance_parameters.pipeline = "standard_green";
	instance_parameters.textures = {};
	instance_parameters.uniform_buffer = uniform_buffer_name;

	std::string instance = create_instance(renderer, instance_parameters);

	/*instance_parameters.uniform_buffer = uniform_buffer_name_2;
	instance_parameters.model = "CUBE";
	std::string instance_2 = create_instance(renderer, instance_parameters);*/

	uint16_t frame_count = 0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		update_image_index(renderer, frame_count);

		static auto start_time = std::chrono::high_resolution_clock::now();

		/*static auto frame_time = std::chrono::high_resolution_clock::now();

		std::cout << 1.0 / std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - frame_time).count() << "\n";

		frame_time = std::chrono::high_resolution_clock::now();*/

		auto current_time = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

		UB ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, .0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		UniformBufferUpdateParameters update_parameters = {};
		update_parameters.buffer_name = uniform_buffer_name;
		update_parameters.data = &ubo;

		update_uniform_buffer(renderer, update_parameters);

		ubo.model = glm::translate(ubo.model, glm::vec3(0.3, 0.3, 0.0));

		/*update_parameters.buffer_name = uniform_buffer_name_2;
		update_uniform_buffer(renderer, update_parameters);*/

		InstanceSubmitParameters submit_parameters = {};
		submit_parameters.instance_name = instance;
		submit_parameters.pipeline_name = "standard_green";

		submit_instance(renderer, submit_parameters);

		/*submit_parameters.instance_name = instance_2;

		submit_instance(renderer, submit_parameters);*/

		DrawParameters draw_parameters = {};
		draw_parameters.draw_frame = frame_count;

		draw(renderer, draw_parameters);

		frame_count++;
		if (frame_count % max_frames == 0)
		{
			frame_count = 0;
		}
	}

	free_uniform_buffer(renderer, uniform_buffer_name);

	cleanup_renderer(renderer);

	glfwDestroyWindow(renderer.window);

	glfwTerminate();

}
