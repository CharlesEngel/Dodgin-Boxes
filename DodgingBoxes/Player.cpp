#include "Player.h"

#include "glm/gtc/matrix_transform.hpp"

Player::Player(Renderer *renderer, Input *input, bool *game_end_flag)
{
	this->renderer = renderer;
	game_end = game_end_flag;
	scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	location = glm::vec2(0.0, 0.0);
	state = PLAYER_DEFAULT;

	current_dash_time = 0;
	current_death_time = 0;

	this->input = input;

	type = 0;

	UniformBufferParameters uniform_parameters = {};
	uniform_parameters.size = sizeof(PlayerUniform);

	uniform_buffer = get_uniform_buffer(*renderer, uniform_parameters);

	LightParameters light_parameters = {};
	light_parameters.color = glm::vec3(0.23, 0.11, 0.96);
	light_parameters.intensity = 0.3f;
	light_parameters.max_distance = 0.8f;
	light_parameters.location = glm::vec3(location, 1.0);
	light_parameters.type = LIGHT_POINT;
	light = create_light(*renderer, light_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.material = MATERIAL_BLUE_CUBE;
	instance_parameters.uniform_buffers = { uniform_buffer };

	instance = create_instance(*renderer, instance_parameters);

	collider.set_placement(location + glm::vec2(-0.1, -0.1), glm::vec2(0.2, 0.2));
}

Player::~Player()
{
	if (renderer->device.device != VK_NULL_HANDLE)
	{
		// Free resources
		free_uniform_buffer(*renderer, uniform_buffer);
		free_instance(*renderer, instance);
		free_light(*renderer, light);
	}
}

void Player::update(double time)
{
	if (state == PLAYER_DEFAULT || state == PLAYER_DASHING)
	{
		glm::vec2 direction(0.0);

		// Find direction
		if (input->up)
		{
			direction += glm::vec2(0.0, 1);
		}
		if (input->down)
		{
			direction += glm::vec2(0.0, -1);
		}
		if (input->left)
		{
			direction += glm::vec2(-1, 0.0);
		}
		if (input->right)
		{
			direction += glm::vec2(1, 0.0);
		}

		// Normalize direction vector
		auto normalized = glm::normalize(direction);

		if (normalized.x < 1)
		{
			direction = normalized;
		}

		// Make sure you can't just hold the dash button
		if (!input->w)
		{
			input_w_released = true;
		}

		// If they press 'w' and you can, dash
		if (input->w && state != PLAYER_DASHING && input_w_released && (direction.x != 0 || direction.y != 0))
		{
			state = PLAYER_DASHING;
			dash_direction = direction;
			current_dash_time = 0.0;
			input_w_released = false;
		}

		if (state != PLAYER_DASHING)
		{
			// Find new location if you are not dashing
			location += glm::vec2(time * speed * direction.x, time * speed * direction.y);
		}
		else
		{
			// Find new location if you are dashing
			location += glm::vec2(time * dash_speed * dash_direction.x, time * dash_speed * dash_direction.y);
			current_dash_time += float(time);
			if (current_dash_time > total_dash_time)
			{
				state = PLAYER_DEFAULT;
			}
		}

		// Stay in the bounds of the screen
		if (location.x >= 0.74)
		{
			location.x = 0.74f;
		}
		if (location.x <= -0.74)
		{
			location.x = -0.74f;
		}
		if (location.y >= 0.74)
		{
			location.y = 0.74f;
		}
		if (location.y <= -0.74)
		{
			location.y = -0.74f;
		}

		// Update collider
		collider.set_placement(location + glm::vec2(-scale_factor / 2.f, -scale_factor / 2.f), glm::vec2(scale_factor, scale_factor));
	}
	else if (state == PLAYER_DYING)
	{
		// Play death animation if dying
		current_death_time += float(time);
		scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor)) * glm::scale(glm::mat4(1), glm::vec3((0.1f + total_death_time - current_death_time) / (0.1f + total_death_time)));

		if (current_death_time > total_death_time)
		{
			// Once you're done with the animation, you're dead
			state = PLAYER_DEAD;
		}

		// Set collider out of bounds so other enemies don't collide with it
		collider.set_placement(glm::vec2(40, 40), glm::vec2(0, 0));
	}
	else
	{
		// Signal that you're dead and the game should end
		*game_end = true;
	}

}

std::vector<Rectangle *> Player::get_collider()
{
	return { &collider };
}

void Player::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	if (state != PLAYER_DEAD)
	{
		update_reflection_map(*renderer, glm::vec3(location, -(0.5 - (scale_factor / 2.f) - 0.001f)));

		// Update light
		LightUpdateParameters light_update_parameters = {};
		light_update_parameters.light_index = light;
		light_update_parameters.color = glm::vec3(0.23, 0.11, 0.96);
		light_update_parameters.intensity = /*2.5f*/ 0.9f /*0.f*/;
		light_update_parameters.max_distance = 1.5f;

		// -(0.5 - (scale_factor / 2.f) - 0.001f) is the distance to put the cube so that the bottom is touching the floor
		light_update_parameters.location = glm::vec3(location, -(0.5 - (scale_factor / 2.f) - 0.001f));
		update_light(*renderer, light_update_parameters);

		// Update uniform buffer
		PlayerUniform uniform_buffer_data;
		uniform_buffer_data.model = glm::translate(glm::mat4(1), glm::vec3(location.x * width, location.y * height, -(0.5 - (scale_factor / 2.f) - 0.001f))) * scale;
		uniform_buffer_data.view = view;
		uniform_buffer_data.proj = proj;
		uniform_buffer_data.light_index = light;

		UniformBufferUpdateParameters update_parameters = {};
		update_parameters.buffer_name = uniform_buffer;
		update_parameters.data = &uniform_buffer_data;

		update_uniform_buffer(*(this->renderer), update_parameters);

		InstanceSubmitParameters submit_parameters = {};
		submit_parameters.instance_name = instance;

		submit_instance(*(this->renderer), submit_parameters);
	}
}

void Player::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{
	if ((state == PLAYER_DEFAULT || state == PLAYER_DASHING) && other->type == 1)
	{
		state = PLAYER_DYING;
	}
}