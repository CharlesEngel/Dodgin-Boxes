#pragma once

#include "Renderer/Renderer.h"
#include "GameObject.h"
#include <chrono>

struct EnemyUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	int light_index;
};

enum EnemyState
{
	ENEMY_DEFAULT = 0,
	ENEMY_DYING = 1,
	ENEMY_DEAD = 2
};

enum EnemyDirection
{
	ENEMY_MOVING_DOWN = 0,
	ENEMY_MOVING_UP = 1,
	ENEMY_MOVING_LEFT = 2,
	ENEMY_MOVING_RIGHT = 3
};

class Enemy : public GameObject
{
public:
	Enemy(Renderer *renderer);
	virtual ~Enemy();

	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);

	EnemyState state;
	glm::vec2 location;

private:
	Renderer *renderer;
	std::string instance;
	std::string uniform_buffer;
	uint8_t light;

	glm::mat4 scale;
	Rectangle collider;

	const float acceleration = 2.2f;
	const float total_death_time = 0.2f;
	const float scale_factor = 0.12f;
	float speed;
	float current_death_time;

	EnemyDirection direction;
private:
	void get_direction();
};