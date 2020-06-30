#include "Enemy.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Utilities.h"

Enemy::Enemy(Renderer *renderer)
{
	this->renderer = renderer;
	scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	state = ENEMY_DEFAULT;
	type = 2;
	current_death_time = 0;

	get_direction();
	
	UniformBufferParameters uniform_parameters = {};
	uniform_parameters.size = sizeof(EnemyUniform);

	uniform_buffer = get_uniform_buffer(*renderer, uniform_parameters);

	LightParameters light_parameters = {};
	light_parameters.color = glm::vec3(0.84, 0.67, 0.23);
	light_parameters.intensity = 0.15f;
	light_parameters.location = glm::vec3(location, -1.0);
	light_parameters.max_distance = 1.0f;
	light_parameters.type = LIGHT_POINT;

	light = create_light(*renderer, light_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.material = MATERIAL_YELLOW_CUBE;
	instance_parameters.uniform_buffers = { { uniform_buffer }, { uniform_buffer } };

	instance = create_instance(*renderer, instance_parameters);

	collider.set_placement(location + glm::vec2(-scale_factor / 2.f, -scale_factor / 2.f), glm::vec2(scale_factor, scale_factor));
}

Enemy::~Enemy()
{
	if (renderer->device.device != VK_NULL_HANDLE)
	{
		free_uniform_buffer(*renderer, uniform_buffer);
		free_instance(*renderer, instance);
		free_light(*renderer, light);
	}
}

void Enemy::update(double time)
{
	if (state == ENEMY_DEFAULT)
	{
		//  If out of bounds, put in new position
		if (direction == 0 && location.y < -1.3)
		{
			get_direction();
		}
		else if (direction == 1 && location.y > 1.3)
		{
			get_direction();
		}
		else if (direction == 2 && location.x < -1.3)
		{
			get_direction();
		}
		else if (direction == 3 && location.x > 1.3)
		{
			get_direction();
		}

		// Update speed
		acceleration += 0.75f * float(time);
		speed += float(time) * acceleration;
		glm::vec2 velocity;

		// Find velocity
		if (direction == 0)
		{
			velocity = glm::vec2(0.0, -speed);
		}
		else if (direction == 1)
		{
			velocity = glm::vec2(0.0, speed);
		}
		else if (direction == 2)
		{
			velocity = glm::vec2(-speed, 0.0);
		}
		else
		{
			velocity = glm::vec2(speed, 0.0);
		}

		//update location
		location += static_cast<float>(time) * velocity;

		// Update position
		collider.set_placement(location + glm::vec2(-scale_factor / 2.f, -scale_factor / 2.f), glm::vec2(scale_factor, scale_factor));
	}
	else if (state == ENEMY_DYING)
	{
		// Play death animation if dying
		current_death_time += float(time);
		scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor)) * glm::scale(glm::mat4(1), glm::vec3((0.1f + total_death_time - current_death_time) / (0.1f + total_death_time)));

		if (current_death_time > total_death_time)
		{
			// When you're dead, you're dead
			state = ENEMY_DEAD;
		}
	}
}

std::vector<Rectangle *> Enemy::get_collider()
{
	return { &collider };
}


void Enemy::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	// Update light
	LightUpdateParameters light_update_parameters = {};
	light_update_parameters.light_index = light;
	light_update_parameters.color = glm::vec3(0.84, 0.67, 0.23);
	light_update_parameters.intensity = 0.4f /*2.5f*/ /*0.f*/;
	light_update_parameters.max_distance = /*0.9f*/ 2.5f;

	light_update_parameters.location = glm::vec3(location, -(0.5 - (scale_factor / 2.f) - 0.001f));
	update_light(*renderer, light_update_parameters);

	// Update uniform buffer
	EnemyUniform buffer_data = {};
	buffer_data.model = glm::translate(glm::mat4(1), glm::vec3(location.x * width, location.y * height, -(0.5 - (scale_factor / 2.f) - 0.001f))) * scale;
	buffer_data.proj = proj;
	buffer_data.view = view;
	buffer_data.light_index = light;

	UniformBufferUpdateParameters update_parameters = {};
	update_parameters.buffer_name = uniform_buffer;
	update_parameters.data = &buffer_data;

	update_uniform_buffer(*renderer, update_parameters);

	InstanceSubmitParameters submit_parameters = {};
	submit_parameters.instance_name = instance;

	submit_instance(*renderer, submit_parameters);
}

void Enemy::get_direction()
{
	// Reset acceleration
	acceleration = start_acceleration;

	// Generate direction
	direction = static_cast<EnemyDirection>(random_int(0, 99) % 4);

	// Find new location on the axis not determined by direction
	auto rand_location = 1.75 * (random_int(0, 100) / 100.0 - 0.5);

	if (direction == 0)
	{
		location = glm::vec2(rand_location, 1.3);
	}
	else if (direction == 1)
	{
		location = glm::vec2(rand_location, -1.3);
	}
	else if (direction == 2)
	{
		location = glm::vec2(1.3, rand_location);
	}
	else
	{
		location = glm::vec2(-1.3, rand_location);
	}

	// Reset speed
	speed = 0;
}

void Enemy::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{
	if (state == ENEMY_DEFAULT && other->type == 0)
	{
		state = ENEMY_DYING;
	}
}