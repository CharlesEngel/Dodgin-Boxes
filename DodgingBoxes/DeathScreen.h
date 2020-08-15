#pragma once
#include "Text.h"

#include <string>

#include "SoundManager.h"

struct DeathUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	float time;
};

struct DeathDarkenUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class DeathScreen : public GameObject
{
public:
	DeathScreen(Renderer *renderer, Font *font, double *score_holder);
	virtual ~DeathScreen();
	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);
	virtual void pause();
	virtual void unpause();

	void write_high_score();

private:
	Renderer *renderer;

	glm::vec3 location;

	Text game_over_text;
	Text score_text;
	Text score_number_text;
	Text high_score_text;
	Text high_score_number_text;
	Text enter_restart_text;
	Text esc_quit_text;
	std::string square_uniform_buffer;
	std::string square_instance;

	std::string screen_darken_uniform_buffer;
	std::string screen_darken_instance;

	double *score;
	double high_score;
	float run_time;
};