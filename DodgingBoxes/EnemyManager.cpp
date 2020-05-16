#include "EnemyManager.h"

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
		if ((*enemy)->dead)
		{
			delete (*enemy);
			uint32_t index = std::distance(enemies.begin(), enemy);
			enemy = enemies.erase(enemy);
			colliders.erase(colliders.begin() + index);
		}
		else
		{
			(*enemy)->update(time);
			enemy++;
		}
	}

	if (enemies.size() < max_enemies && spawn_time > (1.0 + 0.8 * enemies.size()) && (rand() > (RAND_MAX - 15)))
	{
		enemies.push_back(new Enemy(renderer));
		colliders.push_back(enemies.back()->get_collider()[0]);
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

void EnemyManager::handle_internal_collisions()
{
	/*for (uint32_t i = 0; i < colliders.size(); i++)
	{
		for (uint32_t j = i + 1; j < colliders.size(); j++)
		{
			if (check_collision_rect_rect(colliders[i], colliders[j]))
			{
				deaths++;

				enemies[i]->handle_external_collisions(colliders[i], enemies[j]);
				enemies[j]->handle_external_collisions(colliders[j], enemies[i]);
			}
		}
	}*/
}

void EnemyManager::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{
	if (other->type == 0)
	{
		auto element = std::find(colliders.begin(), colliders.end(), collider);
		auto index = std::distance(colliders.begin(), element);

		enemies[index]->handle_external_collisions(collider, other);
	}
}