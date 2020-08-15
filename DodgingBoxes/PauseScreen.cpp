#include "PauseScreen.h"

#include <glm/gtc/matrix_transform.hpp>

PauseScreen::PauseScreen(Renderer *renderer, Font *font)
	: text(renderer, font, glm::vec2(-0.185f, 0.0f), 1.0f, "PAUSED")
{
	this->renderer = renderer;
	location = glm::vec3(0.0, 0.0, 0.0);
	run_time = 0.f;

	UniformBufferParameters buffer_parameters = {};
	buffer_parameters.range = sizeof(SquareUniform);
	buffer_parameters.size = sizeof(SquareUniform);

	square_uniform_buffer = get_uniform_buffer(*renderer, buffer_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.light_index = -1;
	instance_parameters.material = MATERIAL_PAUSE_SCREEN;
	instance_parameters.uniform_buffers = { {square_uniform_buffer} };

	square_instance = create_instance(*renderer, instance_parameters);

	UniformBufferParameters darken_buffer_parameters = {};
	darken_buffer_parameters.range = sizeof(PauseDarkenUniform);
	darken_buffer_parameters.size = sizeof(PauseDarkenUniform);

	screen_darken_uniform_buffer = get_uniform_buffer(*renderer, darken_buffer_parameters);

	InstanceParameters darken_instance_parameters = {};
	darken_instance_parameters.light_index = -1;
	darken_instance_parameters.uniform_buffers = { {screen_darken_uniform_buffer} };
	darken_instance_parameters.material = MATERIAL_DARKEN;

	screen_darken_instance = create_instance(*renderer, darken_instance_parameters);
}

PauseScreen::~PauseScreen()
{
	if (renderer->device.device != VK_NULL_HANDLE)
	{
		// Free resources
		free_uniform_buffer(*renderer, square_uniform_buffer);
		free_instance(*renderer, square_instance);
		free_uniform_buffer(*renderer, screen_darken_uniform_buffer);
		free_instance(*renderer, screen_darken_instance);
	}
}

void PauseScreen::update(double time)
{
	run_time += float(time);
}

std::vector<Rectangle *> PauseScreen::get_collider()
{
	return {};
}

void PauseScreen::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	text.submit_for_rendering(view, proj, width, height);
	
	SquareUniform uniform_data = {};
	uniform_data.model = glm::translate(glm::mat4(1), location) * glm::scale(glm::mat4(1), glm::vec3(0.6, 0.225, 1.0));
	uniform_data.view = view;
	uniform_data.proj = proj;
	uniform_data.time = run_time;

	UniformBufferUpdateParameters uniform_update = {};
	uniform_update.buffer_name = square_uniform_buffer;
	uniform_update.data = &uniform_data;

	update_uniform_buffer(*renderer, uniform_update);

	InstanceSubmitParameters instance_submit = {};
	instance_submit.instance_name = square_instance;

	submit_instance(*renderer, instance_submit);

	PauseDarkenUniform darken_uniform_data = {};
	darken_uniform_data.model = glm::translate(glm::mat4(1), glm::vec3(location.x, location.y, -0.5)) * glm::scale(glm::mat4(1), glm::vec3(2.072, 2.072, 1.0));
	darken_uniform_data.view = view;
	darken_uniform_data.proj = proj;

	UniformBufferUpdateParameters darken_uniform_update = {};
	darken_uniform_update.buffer_name = screen_darken_uniform_buffer;
	darken_uniform_update.data = &darken_uniform_data;

	update_uniform_buffer(*renderer, darken_uniform_update);

	InstanceSubmitParameters darken_instance_submit = {};
	darken_instance_submit.instance_name = screen_darken_instance;

	submit_instance(*renderer, darken_instance_submit);
}

void PauseScreen::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{

}

void PauseScreen::pause()
{

}

void PauseScreen::unpause()
{

}