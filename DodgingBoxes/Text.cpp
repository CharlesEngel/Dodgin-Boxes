#include "Text.h"

#include <glm/gtc/matrix_transform.hpp>

Text::Text(Renderer *renderer, Font *font, glm::vec2 location, float scale_factor, std::string string)
{
	this->renderer = renderer;
	scale = glm::scale(glm::mat4(1), glm::vec3(0.1f, 0.1f, 1.0f));
	this->location = glm::vec3(location, 0.5);
	this->scale_factor = scale_factor;
	this->font = font;

	characters = {};
	inactive_characters = std::stack<Character *>({ new Character(renderer, location, scale_factor, font, ' ', ' ') });

	// Set initial value of the string
	update_string(string);
}

Text::~Text()
{
	for (auto character : characters)
	{
		delete character;
	}

	while (inactive_characters.size() != 0)
	{
		delete inactive_characters.top();
		inactive_characters.pop();
	}
}

void Text::update(double time)
{

}

std::vector<Rectangle *> Text::get_collider()
{
	return {};
}

void Text::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	for (auto character : characters)
	{
		character->submit_for_rendering(view, proj, width, height);
	}
}

void Text::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{

}

void Text::pause()
{
	
}

void Text::unpause()
{
	
}

void Text::update_string(std::string string)
{
	// If you're changing the string to anything new
	if (this->string != string)
	{
		// Update the string value
		this->string = string;

		// If the string is smaller than the previous one
		if (string.size() < characters.size())
		{
			// Move character objects into inactive_characters
			for (uint32_t i = string.size(); i < characters.size(); i++)
			{
				inactive_characters.push(characters.back());
				characters.erase(characters.end());
			}
		}

		// For each character in the string
		for (int i = 0; i < int(string.size()); i++)
		{
			// If we have used all the characters in the characters vector
			if (i > int(characters.size()) - 1)
			{
				// Move a character in from inactive_characters
				characters.push_back(inactive_characters.top());
				inactive_characters.pop();

				if (inactive_characters.size() == 0)
				{
					inactive_characters.push(new Character(this->renderer, glm::vec2(location.x, location.y), scale_factor, font, ' ', ' '));
				}
			}

			// If this is the first character in the string
			if (i == 0)
			{ 
				// Set the first character to the string's location and update the character
				characters[i]->update_location(glm::vec2(location.x, location.y));
				characters[i]->update_character(string[i], 10);
			}
			else
			{
				// Update character
				characters[i]->update_character(string[i], string[i-1]);
				
				// Represents data about the previous character
				CharacterDetails previous_character = font->chars[characters[i - 1]->get_character()];

				// Represents data about the current character
				CharacterDetails current_character = font->chars[characters[i]->get_character()];

				// Find location of current character based on the advance value of the previous character along with the amount the character has been resized
				characters[i]->update_location(characters[i - 1]->get_location() + glm::vec2(0.7 * previous_character.advance / float(font->total_width) + scale_factor * (current_character.width / float(1.25 * font->total_width)), 0));
			}
		}
	}
}

void Text::update_position(glm::vec3 new_location)
{
	location = new_location;
}