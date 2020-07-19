#include "DeathScreen.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Utilities.h"
#include <fstream>

DeathScreen::DeathScreen(Renderer *renderer, Font *font, double *score_holder)
	: game_over_text(renderer, font, glm::vec2(-0.33f, 0.5f), 1.2f, "GAME   OVER"), score_text(renderer, font, glm::vec2(-0.175f, 0.2f), 1.0f, "SCORE:"), score_number_text(renderer, font, glm::vec2(0.0f, 0.075f), 0.9f, "0"), high_score_text(renderer, font, glm::vec2(-0.325f, -0.1f), 1.0f, "HIGH   SCORE:"), high_score_number_text(renderer, font, glm::vec2(0.0f, -0.225f), 0.9f, "0"), esc_restart_text(renderer, font, glm::vec2(-0.38, -0.45), 0.75f, "ESC   TO   RESTART"), enter_quit_text(renderer, font, glm::vec2(-0.32, -0.535), 0.65f, "ENTER   TO   QUIT")
{
	this->renderer = renderer;
	location = glm::vec3(0.0, 0.0, 0.0);
	score = score_holder;
	run_time = 0.f;

	UniformBufferParameters buffer_parameters = {};
	buffer_parameters.range = sizeof(DeathUniform);
	buffer_parameters.size = sizeof(DeathUniform);

	square_uniform_buffer = get_uniform_buffer(*renderer, buffer_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.light_index = -1;
	instance_parameters.material = MATERIAL_DEATH_SCREEN;
	instance_parameters.uniform_buffers = { {square_uniform_buffer} };

	square_instance = create_instance(*renderer, instance_parameters);

	UniformBufferParameters darken_buffer_parameters = {};
	darken_buffer_parameters.range = sizeof(DeathDarkenUniform);
	darken_buffer_parameters.size = sizeof(DeathDarkenUniform);

	screen_darken_uniform_buffer = get_uniform_buffer(*renderer, darken_buffer_parameters);

	InstanceParameters darken_instance_parameters = {};
	darken_instance_parameters.light_index = -1;
	darken_instance_parameters.uniform_buffers = { {screen_darken_uniform_buffer} };
	darken_instance_parameters.material = MATERIAL_DARKEN;

	screen_darken_instance = create_instance(*renderer, darken_instance_parameters);

	std::ifstream read;
	read.open("Resources/save_data.b");
	read.read((char *)(&high_score), sizeof(high_score));
	read.close();
}

DeathScreen::~DeathScreen()
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

void DeathScreen::update(double time)
{
	run_time += float(time);

	score_number_text.update_position(glm::vec3(-0.02 * num_digits(int(*score)), 0.075, 0.9f));
	score_number_text.update_string(std::to_string(int(*score)));

	if (*score > high_score)
	{
		high_score = *score;
	}

	high_score_number_text.update_position(glm::vec3(-0.02 * num_digits(int(high_score)), -0.225f, 0.9f));
	high_score_number_text.update_string(std::to_string(int(high_score)));
}

std::vector<Rectangle *> DeathScreen::get_collider()
{
	return {};
}

void DeathScreen::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	game_over_text.submit_for_rendering(view, proj, width, height);
	score_text.submit_for_rendering(view, proj, width, height);
	high_score_text.submit_for_rendering(view, proj, width, height);
	score_number_text.submit_for_rendering(view, proj, width, height);
	high_score_number_text.submit_for_rendering(view, proj, width, height);
	esc_restart_text.submit_for_rendering(view, proj, width, height);
	enter_quit_text.submit_for_rendering(view, proj, width, height);

	DeathUniform uniform_data = {};
	uniform_data.model = glm::translate(glm::mat4(1), location) * glm::scale(glm::mat4(1), glm::vec3(0.9, 1.23, 1.0));
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

	DeathDarkenUniform darken_uniform_data = {};
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

void DeathScreen::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{

}

void DeathScreen::write_high_score()
{
	std::ofstream write;
	write.open("Resources/save_data.b");
	write.write((char*)&high_score, sizeof(high_score));
	write.close();
}