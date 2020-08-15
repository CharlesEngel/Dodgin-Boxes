#pragma once
#include "Text.h"

#include <string>

#include "SoundManager.h"

struct SquareUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	float time;
};

struct PauseDarkenUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class PauseScreen : public GameObject
{
public:
	PauseScreen(Renderer *renderer, Font *font);
	virtual ~PauseScreen();
	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);
	virtual void pause();
	virtual void unpause();

private:
	Renderer *renderer;

	glm::vec3 location;

	Text text;
	std::string square_uniform_buffer;
	std::string square_instance;

	std::string screen_darken_uniform_buffer;
	std::string screen_darken_instance;

	float run_time;
};