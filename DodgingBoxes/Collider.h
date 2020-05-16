#pragma once

#include <stdint.h>
#include <glm/glm.hpp>

class Rectangle
{
public:
	Rectangle();
	void set_placement(glm::vec2 position, glm::vec2 size);
	glm::vec2 position;
	glm::vec2 size;
};

// Check collision between two rectangles
bool check_collision_rect_rect(const Rectangle *collider_1, const Rectangle *collider_2);