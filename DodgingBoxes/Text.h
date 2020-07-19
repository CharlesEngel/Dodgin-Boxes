#pragma once
#include "Font.h"
#include "Character.h"

#include <string>
#include <stack>

class Text : public GameObject
{
public:
	Text(Renderer *renderer, Font *font, glm::vec2 location, float scale_factor, std::string string);
	virtual ~Text();
	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);

	void update_string(std::string string);
	void update_position(glm::vec3 new_location);
private:
	Renderer *renderer;

	glm::mat4 scale;
	glm::vec3 location;

	Font *font;
	std::string string;
	std::vector<Character *> characters;
	std::stack<Character *> inactive_characters;
	float scale_factor;
};