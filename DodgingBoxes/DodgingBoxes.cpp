#include "Renderer/Renderer.h"
#include "Player.h"

#include <chrono>
#include <iostream>
#include <time.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

const int max_frames = 2;

const int width = 800;
const int height = 600;

const std::vector<std::string> models = {
	
};

const std::vector<std::string> shaders = {
	"vert_standard.spv",
	"frag_green.spv",
	"frag_red.spv",
	"frag_blue.spv",
	"frag_yellow.spv"
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

	glfwSetKeyCallback(window, GameManager::handle_input);

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

	GameManager game_manager(&renderer, width, height);

	uint16_t frame_count = 0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		update_image_index(renderer, frame_count);

		static auto frame_time_1 = std::chrono::high_resolution_clock::now();
		static auto frame_time_2 = std::chrono::high_resolution_clock::now();
		double time;

		if (frame_time_1 > frame_time_2)
		{
			frame_time_2 = std::chrono::high_resolution_clock::now();
			time = std::chrono::duration<float, std::chrono::seconds::period>(frame_time_2 - frame_time_1).count();
		}
		else
		{
			frame_time_1 = std::chrono::high_resolution_clock::now();
			time = std::chrono::duration<float, std::chrono::seconds::period>(frame_time_1 - frame_time_2).count();
		}

		int w, h;

		glfwGetFramebufferSize(window, &w, &h);

		game_manager.update(time, w, h);
		game_manager.resolve_collisions();
		//std::cout << 1.0 / time << "\n";

		game_manager.submit_for_rendering(static_cast<uint32_t>(w), static_cast<uint32_t>(h));

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

	cleanup_renderer(renderer);

	glfwDestroyWindow(renderer.window);

	glfwTerminate();

}
