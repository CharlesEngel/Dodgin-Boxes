#pragma once

#include "Renderer/Renderer.h"
#include "GameObject.h"
#include <glm/mat4x4.hpp>

#include "GameManager.h"

struct PlayerUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

enum PlayerState
{
	PLAYER_DEFAULT = 0,
	PLAYER_DASHING = 1,
	PLAYER_DEAD = 2
};

class Player : public GameObject
{
public:
	Player(Renderer *renderer, Input *input);
	~Player();
	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);

	glm::vec2 location;
private:
	Renderer *renderer;
	std::string instance;
	std::string uniform_buffer;

	glm::mat4 scale;
	Rectangle collider;
	const Input *input;

	const float speed = 0.8f;
	const float dash_speed = 3.f;
	const float total_dash_time = 0.08f;

	glm::vec2 dash_direction;
	float current_dash_time;

	PlayerState state;

	bool input_w_released;
};