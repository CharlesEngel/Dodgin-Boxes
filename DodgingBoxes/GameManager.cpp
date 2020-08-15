#pragma once
#include "GameManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Text.h"

#include "glm/gtc/matrix_transform.hpp"

Input GameManager::input = {false, false, false, false, false};

GameManager::GameManager(Renderer *renderer, uint32_t width, uint32_t height)
{
	this->renderer = renderer;
	game_should_end = false;
	start_new_game = false;
	score = 0.0;

	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);
	proj[1][1] *= -1;

	view_height = 2.5f * tan(glm::radians(45.0f / 2.0f));
	view_width = view_height;

	font = new Font(FONT_ARIAL);

	objects.push_back(new Player(renderer, &input, &game_should_end));
	objects.push_back(new EnemyManager(renderer, font, &score));

	transform = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0.0, 0.0, -0.5)), glm::vec3(2.071, 2.071, 1.0));

	UniformBufferParameters uniform_parameters = {};
	uniform_parameters.size = sizeof(FloorVertUniform);
	
	vert_uniform_buffer = get_uniform_buffer(*renderer, uniform_parameters);

	for (uint32_t i = 0; i < 196; i++)
	{
		active_tiles[i] = 0.f;
	}

	uniform_parameters.size = sizeof(FloorFragUniform);
	frag_uniform_buffer = get_uniform_buffer(*renderer, uniform_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.material = MATERIAL_RED_SQUARE;
	instance_parameters.uniform_buffers = { { vert_uniform_buffer, frag_uniform_buffer }, { vert_uniform_buffer, frag_uniform_buffer } };

	instance = create_instance(*renderer, instance_parameters);

	state = GAME_STATE_DEFAULT;

	pause_screen = new PauseScreen(renderer, font);
	death_screen = new DeathScreen(renderer, font, &score);

	sound_manager = &SoundManager::get_instance();

	menu_sound = sound_manager->register_sound(SOUND_TYPE_MENU);
	sound_manager->update_sound_gain(menu_sound, 1.15f);

	music_sound = sound_manager->register_sound(SOUND_TYPE_MUSIC);
	sound_manager->update_sound_loop(music_sound, true);
	sound_manager->update_sound_relative(music_sound, true);
	sound_manager->update_sound_position(music_sound, 0.f, 0.f, 0.f);
	sound_manager->update_sound_velocity(music_sound, 0.f, 0.f, 0.f);
	sound_manager->update_sound_gain(music_sound, 0.3f);
	sound_manager->play_sound(music_sound);
}

GameManager::~GameManager()
{
	delete font;
	delete pause_screen;
	delete death_screen;

	sound_manager->stop_sound(music_sound);
	sound_manager->delete_sound(music_sound);

	if (renderer->device.device != VK_NULL_HANDLE)
	{
		free_uniform_buffer(*renderer, vert_uniform_buffer);
		free_uniform_buffer(*renderer, frag_uniform_buffer);
		free_instance(*renderer, instance);
	}

	for (auto object : objects)
	{
		delete object;
	}
}

void GameManager::handle_input(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_UP)
		{
			input.up = true;
		}
		else if (key == GLFW_KEY_DOWN)
		{
			input.down = true;
		}
		else if (key == GLFW_KEY_LEFT)
		{
			input.left = true;
		}
		else if (key == GLFW_KEY_RIGHT)
		{
			input.right = true;
		}
		else if (key == GLFW_KEY_W)
		{
			input.w = true;
		}
		else if (key == GLFW_KEY_ESCAPE)
		{
			input.esc = true;
		}
		else if (key == GLFW_KEY_ENTER)
		{
			input.enter = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_UP)
		{
			input.up = false;
		}
		else if (key == GLFW_KEY_DOWN)
		{
			input.down = false;
		}
		else if (key == GLFW_KEY_LEFT)
		{
			input.left = false;
		}
		else if (key == GLFW_KEY_RIGHT)
		{
			input.right = false;
		}
		else if (key == GLFW_KEY_W)
		{
			input.w = false;
		}
		else if (key == GLFW_KEY_ESCAPE)
		{
			input.esc = false;
		}
		else if (key == GLFW_KEY_ENTER)
		{
			input.enter = false;
		}
	}
}

void GameManager::update(double time, uint32_t width, uint32_t height)
{
	if (game_should_end && state != GAME_STATE_OVER)
	{
		state = GAME_STATE_OVER;
		sound_manager->update_sound_gain(music_sound, 0.1f);
		death_screen->write_high_score();
	}

	if (!input.esc)
	{
		esc_released = true;
	}
	else if (esc_released)
	{
		if (state == GAME_STATE_PAUSED)
		{
			state = GAME_STATE_DEFAULT;
			sound_manager->update_sound_gain(music_sound, 0.3f);
			play_menu_sound();

			for (auto &object : objects)
			{
				object->unpause();
			}
		}
		else if (state == GAME_STATE_DEFAULT)
		{
			state = GAME_STATE_PAUSED;
			sound_manager->update_sound_gain(music_sound, 0.1f);
			play_menu_sound();

			for (auto &object : objects)
			{
				object->pause();
			}
		}
		else if (state == GAME_STATE_OVER)
		{
			user_quit = true;
		}

		esc_released = false;
	}

	if (state == GAME_STATE_OVER && input.enter)
	{
		start_new_game = true;
		play_menu_sound();
	}


	if (state == GAME_STATE_DEFAULT)
	{
		// For every object
		for (uint32_t i = 0; i < objects.size(); i++)
		{
			// For every collider the object contains
			for (auto& collider : objects[i]->get_collider())
			{
				// For every floor tile
				for (uint32_t k = 0; k < 14; k++)
				{
					for (uint32_t f = 0; f < 14; f++)
					{
						// Check if they collide
						Rectangle floor_rect;
						floor_rect.set_placement(glm::vec2(-halfWidth + k * tileWidth, -halfWidth + f * tileWidth), glm::vec2(tileWidth, tileWidth));
						if (check_collision_rect_rect(collider, &floor_rect))
						{
							active_tiles[k * 14 + f] = 1.f;
						}
					}
				}
			}
		}

		// For every floor tile
		for (uint32_t k = 0; k < 14; k++)
		{
			for (uint32_t f = 0; f < 14; f++)
			{
				// Subtract the time
				active_tiles[k * 14 + f] = std::max(0.0f, active_tiles[k * 14 + f] - float(time));
			}
		}

		for (auto object : objects)
		{
			object->update(time);
		}
	}

	death_screen->update(time);
	pause_screen->update(time);
}

void GameManager::resolve_collisions()
{
	if (state == GAME_STATE_DEFAULT)
	{
		for (uint32_t i = 0; i < objects.size(); i++)
		{
			for (uint32_t j = i + 1; j < objects.size(); j++)
			{
				for (auto &collider_1 : objects[i]->get_collider())
				{
					for (auto &collider_2 : objects[j]->get_collider())
					{
						if (check_collision_rect_rect(collider_1, collider_2))
						{
							objects[i]->handle_external_collisions(collider_1, objects[j]);
							objects[j]->handle_external_collisions(collider_2, objects[i]);
						}
					}
				}
			}
		}
	}
}

void GameManager::submit_for_rendering(uint32_t width, uint32_t height)
{
	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 10.0f);
	proj[1][1] *= -1;

	if (state == GAME_STATE_PAUSED)
	{
		pause_screen->submit_for_rendering(view, proj, view_width, view_height);
	}
	else if (state == GAME_STATE_OVER)
	{
		death_screen->submit_for_rendering(view, proj, view_width, view_height);
	}

	for (auto object : objects)
	{
		object->submit_for_rendering(view, proj, view_width, view_height);
	}

	FloorVertUniform buffer_data = {};
	buffer_data.model = transform;
	buffer_data.proj = proj;
	buffer_data.view = view;
	buffer_data.light_index = -1;

	UniformBufferUpdateParameters update_parameters = {};
	update_parameters.buffer_name = vert_uniform_buffer;
	update_parameters.data = &buffer_data;

	update_uniform_buffer(*renderer, update_parameters);

	FloorFragUniform frag_buffer_data = {};
	for (uint32_t i = 0; i < 196; i++)
	{
		frag_buffer_data.active_tiles[i].value = active_tiles[i];
	}

	update_parameters.buffer_name = frag_uniform_buffer;
	update_parameters.data = &frag_buffer_data;

	update_uniform_buffer(*renderer, update_parameters);

	InstanceSubmitParameters submit_parameters = {};
	submit_parameters.instance_name = instance;

	submit_instance(*renderer, submit_parameters);
}

bool GameManager::game_has_ended() const
{
	return start_new_game;
}

bool GameManager::should_quit() const
{
	return user_quit;
}

void GameManager::play_menu_sound()
{
	sound_manager->update_sound_relative(menu_sound, true);
	sound_manager->update_sound_position(menu_sound, 0.f, 0.f, 0.f);
	sound_manager->update_sound_velocity(menu_sound, 0.f, 0.f, 0.f);

	sound_manager->play_sound(menu_sound);
}