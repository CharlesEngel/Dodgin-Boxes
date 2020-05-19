#include "EnemyManager.h"

#include <iostream>
#include "Utilities.h"

EnemyManager::EnemyManager(Renderer *renderer)
{
	this->renderer = renderer;
	type = 1;

	enemies.push_back(new Enemy(renderer));
	colliders.push_back(enemies[0]->get_collider()[0]);

	spawn_time = 0;
}

EnemyManager::~EnemyManager()
{
	for (auto enemy : enemies)
	{
		delete enemy;
	}
}

void EnemyManager::update(double time)
{
	spawn_time += time;

	auto enemy = enemies.begin();
	while (enemy != enemies.end())
	{
		if ((*enemy)->state == ENEMY_DEAD)
		{
			// Remove enemy if it is dead
			delete (*enemy);
			uint32_t index = std::distance(enemies.begin(), enemy);
			enemy = enemies.erase(enemy);
			colliders.erase(colliders.begin() + index);
		}
		else
		{
			// Update enemy if it is not dead
			(*enemy)->update(time);
			enemy++;
		}
	}

	if (spawn_time > (1.0 + 0.5 * enemies.size()))
	{
		if (enemies.size() < max_enemies && (random_int(0, 100) > 80))
		{
			// If conditions line up, create enemy
			enemies.push_back(new Enemy(renderer));
			colliders.push_back(enemies.back()->get_collider()[0]);
		}
		spawn_time = 0;
	}
}

std::vector<Rectangle *> EnemyManager::get_collider()
{
	return colliders;
}

void EnemyManager::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	for (auto &enemy : enemies)
	{
		enemy->submit_for_rendering(view, proj, width, height);
	}
}

void EnemyManager::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{
	if (other->type == 0)
	{
		// Find which enemy collided
		auto element = std::find(colliders.begin(), colliders.end(), collider);
		auto index = std::distance(colliders.begin(), element);

		// Tell enemy to handle collision
		enemies[index]->handle_external_collisions(collider, other);
	}
}