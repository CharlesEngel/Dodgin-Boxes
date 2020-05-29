#pragma once

#include "Renderer/Renderer.h"
#include "GameObject.h"
#include <glm/mat4x4.hpp>
#include "Font.h"

struct CharacterUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	float x;
	float y;
	float width;
	float height;
};

class Character : public GameObject
{
public:
	Character(Renderer *renderer, glm::vec2 location, float scale_factor, Font *font, char character, char previous_character);
	virtual ~Character();
	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);

	void update_character(char character, char previous_character);
	glm::vec2 get_location();
	void update_location(glm::vec2 new_location);
	char get_character();

private:
	Renderer *renderer;
	std::string instance;
	std::string uniform_buffer;

	glm::mat4 scale;
	float string_scale_factor;
	glm::vec3 location;

	Font *font;
	char character;
	char previous_character;
};