#pragma once
#include "GameManager.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Floor.h"

#include "glm/gtc/matrix_transform.hpp"

Input GameManager::input = {false, false, false, false, false};

GameManager::GameManager(Renderer *renderer, uint32_t width, uint32_t height)
{
	this->renderer = renderer;

	objects.push_back(new Player(renderer, &input));
	objects.push_back(new EnemyManager(renderer));
	objects.push_back(new Floor(renderer));

	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::perspective(glm::radians(45.0f), /*width / (float)height*/ 1.0f, 0.1f, 10.0f);
	proj[1][1] *= -1;

	view_height = 2.25f * tan(glm::radians(45.0f / 2.0f));
	view_width = view_height;
}

GameManager::~GameManager()
{
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
	}
}

void GameManager::update(double time, uint32_t width, uint32_t height)
{
	for (auto object : objects)
	{
		object->update(time);
	}
}

void GameManager::resolve_collisions()
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

void GameManager::submit_for_rendering(uint32_t width, uint32_t height)
{
	proj = glm::perspective(glm::radians(45.0f), /* This will probably need to change: */1.0f, 0.1f, 10.0f);
	proj[1][1] *= -1;

	for (auto object : objects)
	{
		object->submit_for_rendering(view, proj, view_width, view_height);
	}
}
