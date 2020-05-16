#include "Player.h"

#include "glm/gtc/matrix_transform.hpp"

Player::Player(Renderer *renderer, Input *input)
{
	this->renderer = renderer;
	scale = glm::scale(glm::mat4(1), glm::vec3(0.2, 0.2, 0.2));
	location = glm::vec2(0.0, 0.0);
	dead = false;

	this->input = input;

	type = 0;

	UniformBufferParameters uniform_parameters = {};
	uniform_parameters.size = sizeof(PlayerUniform);

	uniform_buffer = get_uniform_buffer(*renderer, uniform_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.material = 2;
	instance_parameters.uniform_buffer = uniform_buffer;

	instance = create_instance(*renderer, instance_parameters);

	collider.set_placement(location + glm::vec2(-0.1, -0.1), glm::vec2(0.2, 0.2));
}

Player::~Player()
{
	if (renderer->device.device != VK_NULL_HANDLE)
	{
		free_uniform_buffer(*renderer, uniform_buffer);
		free_instance(*renderer, instance);
	}
}

void Player::update(double time)
{
	if (!dead)
	{
		glm::vec2 direction(0.0);

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

		auto normalized = glm::normalize(direction);

		if (normalized.x < 1)
		{
			direction = normalized;
		}

		if (!input->w)
		{
			input_w_released = true;
		}

		if (input->w && !dash && input_w_released && (direction.x != 0 || direction.y != 0))
		{
			dash = true;
			dash_direction = direction;
			current_dash_time = 0.0;
			input_w_released = false;
		}

		if (!dash)
		{
			location += glm::vec2(time * speed * direction.x, time * speed * direction.y);
		}
		else
		{
			location += glm::vec2(time * dash_speed * dash_direction.x, time * dash_speed * dash_direction.y);
			current_dash_time += float(time);
			if (current_dash_time > total_dash_time)
			{
				dash = false;
			}
		}

		if (location.x >= 0.75)
		{
			location.x = 0.75f;
		}
		if (location.x <= -0.75)
		{
			location.x = -0.75f;
		}
		if (location.y >= 0.75)
		{
			location.y = 0.75f;
		}
		if (location.y <= -0.75)
		{
			location.y = -0.75f;
		}

		collider.set_placement(location + glm::vec2(-0.1, -0.1), glm::vec2(0.2, 0.2));
	}
	else
	{
		collider.set_placement(glm::vec2(40, 40), glm::vec2(0, 0));
	}

}

std::vector<Rectangle *> Player::get_collider()
{
	return { &collider };
}

void Player::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	PlayerUniform uniform_buffer_data;
	uniform_buffer_data.model = glm::translate(glm::mat4(1), glm::vec3(location.x * width, location.y * height, 0.0)) * scale;
	uniform_buffer_data.view = view;
	uniform_buffer_data.proj = proj;

	UniformBufferUpdateParameters update_parameters = {};
	update_parameters.buffer_name = uniform_buffer;
	update_parameters.data = &uniform_buffer_data;

	update_uniform_buffer(*(this->renderer), update_parameters);

	InstanceSubmitParameters submit_parameters = {};
	submit_parameters.instance_name = instance;
	
	submit_instance(*(this->renderer), submit_parameters);
}

void Player::handle_internal_collisions()
{

}

void Player::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{
	if (other->type == 1)
	{
		dead = true;
	}
}