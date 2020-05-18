#include "Enemy.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Utilities.h"

Enemy::Enemy(Renderer *renderer)
{
	this->renderer = renderer;
	scale = glm::scale(glm::mat4(1), glm::vec3(0.08, 0.08, 0.08));
	dead = false;
	type = 2;

	get_direction();
	
	UniformBufferParameters uniform_parameters = {};
	uniform_parameters.size = sizeof(EnemyUniform);

	uniform_buffer = get_uniform_buffer(*renderer, uniform_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.material = 3;
	instance_parameters.uniform_buffer = uniform_buffer;

	instance = create_instance(*renderer, instance_parameters);

	collider.set_placement(location + glm::vec2(-0.04, -0.04), glm::vec2(0.08, 0.08));
}

Enemy::~Enemy()
{
	if (renderer->device.device != VK_NULL_HANDLE)
	{
		free_uniform_buffer(*renderer, uniform_buffer);
		free_instance(*renderer, instance);
	}
}

void Enemy::update(double time)
{
	if (direction == 0 && location.y < -1)
	{
		get_direction();
	}
	else if (direction == 1 && location.y > 1)
	{
		get_direction();
	}
	else if (direction == 2 && location.x < -1)
	{
		get_direction();
	}
	else if (direction == 3 && location.x > 1)
	{
		get_direction();
	}

	speed += float(time) * acceleration;
	glm::vec2 velocity;

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

	location += static_cast<float>(time) * velocity;

	collider.set_placement(location + glm::vec2(-0.04, -0.04), glm::vec2(0.08, 0.08));
}

std::vector<Rectangle *> Enemy::get_collider()
{
	return { &collider };
}


void Enemy::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	EnemyUniform buffer_data = {};
	buffer_data.model = glm::translate(glm::mat4(1), glm::vec3(location.x * width, location.y * height, -0.075)) * scale;
	buffer_data.proj = proj;
	buffer_data.view = view;

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
	direction = static_cast<EnemyDirection>(random_int(0, 99) % 4);

	auto rand_location = 1.6 * (random_int(0, 100) / 100.0 - 0.5);

	if (direction == 0)
	{
		location = glm::vec2(rand_location, 1);
	}
	else if (direction == 1)
	{
		location = glm::vec2(rand_location, -1);
	}
	else if (direction == 2)
	{
		location = glm::vec2(1, rand_location);
	}
	else
	{
		location = glm::vec2(-1, rand_location);
	}

	speed = 0;
}

void Enemy::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{
	if (other->type == 0)
	{
		dead = true;
	}
}