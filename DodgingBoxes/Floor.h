#pragma once

#include "Renderer/Renderer.h"
#include "GameObject.h"

struct FloorUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class Floor : public GameObject
{
public:
	Floor(Renderer *renderer);
	virtual ~Floor();

	virtual void update(double time);
	virtual std::vector<Rectangle *> get_collider();
	virtual void submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const;
	virtual void handle_external_collisions(const Rectangle *collider, const GameObject *other);

private:
	Renderer *renderer;
	std::string instance;
	std::string uniform_buffer;

	glm::mat4 transform;
};
