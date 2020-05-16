#include "Collider.h"

Rectangle::Rectangle()
{
	position = glm::vec2(0, 0);
	size = glm::vec2(0, 0);
}

void Rectangle::set_placement(glm::vec2 position, glm::vec2 size)
{
	this->position = position;
	this->size = size;
}

bool check_collision_rect_rect(const Rectangle *collider_1, const Rectangle *collider_2)
{
	if (collider_1->position.x < collider_2->position.x + collider_2->size.x &&
		collider_2->position.x < collider_1->position.x + collider_1->size.x &&
		collider_1->position.y < collider_2->position.y + collider_2->size.y &&
		collider_2->position.y < collider_1->position.y + collider_1->size.y)
	{
		return true;
	}

	return false;
}