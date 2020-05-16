#pragma once

#include <glm/mat4x4.hpp>
#include <vector>
#include "Collider.h"

class GameObject
{
public:
	virtual void update(double time) = 0;
	virtual std::vector<Rectangle *> get_collider() = 0;
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const = 0;
	virtual void handle_internal_collisions() = 0;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other) = 0;

	glm::vec2 location;
	uint32_t type;
};