#pragma once

#include "Renderer/Renderer.h"
#include "GameObject.h"
#include <chrono>

struct EnemyUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
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
	~Enemy();

	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);

	bool dead;
	glm::vec2 location;

private:
	Renderer *renderer;
	std::string instance;
	std::string uniform_buffer;

	glm::mat4 scale;
	Rectangle collider;

	const float acceleration = 1.5f;
	float speed;

	EnemyDirection direction;
private:
	void get_direction();
};