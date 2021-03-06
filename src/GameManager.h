#pragma once

#include "GameObject.h"
#include "Renderer/Renderer.h"
#include "PauseScreen.h"
#include "DeathScreen.h"

enum GameState
{
	GAME_STATE_DEFAULT = 0,
	GAME_STATE_PAUSED = 1,
	GAME_STATE_OVER = 2
};

struct Input
{
	bool up, down, left, right;
	bool w;
	bool esc;
	bool enter;
};

struct FloorVertUniform
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	int light_index;
};

struct FloorFragUniform
{
	AlignedFloat active_tiles[196];
};

// Width of floor tile
const float tileWidth = (5.f * tan(glm::radians(45.f / 2.f))) / 14;

// Width of half the floor 
const float halfWidth = (2.5f * tan(glm::radians(45.f / 2.f)));

class GameManager
{
public:
	GameManager(Renderer *renderer, uint32_t width, uint32_t height);
	~GameManager();

	static void handle_input(GLFWwindow *window, int key, int scancode, int action, int mods);
	void update(double time, uint32_t width, uint32_t height);
	void resolve_collisions();
	void submit_for_rendering(uint32_t width, uint32_t height);
	bool game_has_ended() const;
	bool should_quit() const;

	GameManager(const GameManager&) = delete;

private:
	void play_menu_sound();

	std::vector<GameObject *> objects;
	Renderer *renderer;
	static Input input;
	std::string vert_uniform_buffer;
	std::string frag_uniform_buffer;
	std::string instance;

	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 transform;
	float view_width;
	float view_height;
	float active_tiles[196];
	double score;

	Font *font;

	PauseScreen *pause_screen;
	DeathScreen *death_screen;

	GameState state;

	SoundManager *sound_manager;
	size_t menu_sound;
	size_t music_sound;

	bool game_should_end;
	bool start_new_game;
	bool esc_released;
	bool user_quit;
};