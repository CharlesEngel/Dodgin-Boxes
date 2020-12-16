#pragma once

#include "Renderer/Renderer.h"
#include "GameObject.h"
#include <glm/mat4x4.hpp>

#include "SoundManager.h"

#include "GameManager.h"

struct PlayerUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	int light_index;
};

enum PlayerState
{
	PLAYER_DEFAULT = 0,
	PLAYER_DASHING = 1,
	PLAYER_DYING = 2,
	PLAYER_DEAD = 3
};

class Player : public GameObject
{
public:
	Player(Renderer *renderer, Input *input, bool *game_end_flag);
	virtual ~Player();
	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);
	virtual void pause();
	virtual void unpause();

	glm::vec2 location;
private:
	Renderer *renderer;
	std::string instance;
	std::string uniform_buffer;
	uint8_t light;
	glm::vec2 light_location;

	glm::mat4 scale;
	Rectangle collider;
	const Input *input;
	bool *game_end;

	const float speed = 1.2f;
	const float dash_speed = 3.8f;
	const float total_dash_time = 0.10f;
	const float total_death_time = 0.3f;
	const float scale_factor = 0.3f;

	glm::vec2 dash_direction;
	float current_dash_time;
	float current_death_time;

	PlayerState state;

	size_t dash_sound;
	bool resume_dash;
	size_t death_sound;
	bool resume_death;
	SoundManager *sound_manager;

	bool input_w_released;
};