#pragma once

#include "GameObject.h"
#include "Renderer/Renderer.h"

struct Input
{
	bool up, down, left, right;
	bool w;
};

class GameManager
{
public:
	GameManager(Renderer *renderer, uint32_t width, uint32_t height);
	~GameManager();

	static void handle_input(GLFWwindow *window, int key, int scancode, int action, int mods);
	void update(double time, uint32_t width, uint32_t height);
	void resolve_collisions();
	void submit_for_rendering(uint32_t width, uint32_t height);
	bool game_has_ended();

	GameManager(const GameManager&) = delete;

private:
	std::vector<GameObject *> objects;
	Renderer *renderer;
	static Input input;

	glm::mat4 view;
	glm::mat4 proj;
	float view_width;
	float view_height;

	bool game_should_end;
};