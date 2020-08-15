#include "Character.h"

#include <glm/gtc/matrix_transform.hpp>

Character::Character(Renderer *renderer, glm::vec2 location, float scale_factor, Font *font, char character, char previous_character)
{
	this->renderer = renderer;
	scale = glm::scale(glm::mat4(1), glm::vec3(0.1f, 0.1f, 1.0f));
	this->location = glm::vec3(location, 0.5);
	string_scale_factor = scale_factor;

	UniformBufferParameters uniform_parameters = {};
	uniform_parameters.size = sizeof(CharacterUniform);

	uniform_buffer = get_uniform_buffer(*renderer, uniform_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.material = MATERIAL_TEXT;
	instance_parameters.uniform_buffers = { { uniform_buffer } };

	instance = create_instance(*renderer, instance_parameters);

	this->font = font;
	update_character(character, previous_character);
}

Character::~Character()
{
	if (renderer->device.device != VK_NULL_HANDLE)
	{
		free_uniform_buffer(*renderer, uniform_buffer);
		free_instance(*renderer, instance);
	}
}

void Character::update(double time)
{

}

std::vector<Rectangle *> Character::get_collider()
{
	return {};
}

void Character::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	// Details about the character from the font
	CharacterDetails char_details = font->chars[character];

	// Total width of the font texture
	float total_width = float(font->total_width);

	// Total height of the font texture
	float total_height = float(font->total_height);

	// How much to modify the x-value for kerning
	float kerning_width = float(char_details.kerning_details[previous_character]);

	// Need to calculate width and height ourselves because we're closer to the camera than the rest of the geometry
	float view_height = 1.5f * tan(glm::radians(45.0f / 2.0f));
	float view_width = view_height;

	// Calculate location of the character
	glm::vec3 translate_vector = location + glm::vec3(char_details.x_offset / total_width + kerning_width / total_width, -0.8 * char_details.y_offset / total_height, 0.f);
	translate_vector = glm::vec3(translate_vector.x * view_width, translate_vector.y * view_height, translate_vector.z);

	// Create translation matrix
	glm::mat4 translate = glm::translate(glm::mat4(1), translate_vector);

	CharacterUniform buffer_data = {};
	buffer_data.model = translate * scale;
	buffer_data.proj = proj;
	buffer_data.view = view;
	buffer_data.x = char_details.x /total_width;
	buffer_data.y = char_details.y / total_height;
	buffer_data.width = char_details.width / total_width;
	buffer_data.height = char_details.height / total_height;

	UniformBufferUpdateParameters update_parameters = {};
	update_parameters.buffer_name = uniform_buffer;
	update_parameters.data = &buffer_data;

	update_uniform_buffer(*renderer, update_parameters);

	InstanceSubmitParameters submit_parameters = {};
	submit_parameters.instance_name = instance;

	submit_instance(*renderer, submit_parameters);
}

void Character::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{

}

void Character::pause()
{

}

void Character::unpause()
{

}


void Character::update_character(char character, char previous_character)
{
	this->character = character;
	this->previous_character = previous_character;

	// Scale draw rect based on the width and height of the character
	scale = glm::scale(glm::mat4(1), glm::vec3(string_scale_factor * font->chars[character].width / float(font->total_width), string_scale_factor * font->chars[character].height / float(font->total_height), 1.0));
}

glm::vec2 Character::get_location()
{
	return location;
}

void Character::update_location(glm::vec2 new_location)
{
	location = glm::vec3(new_location, 0);
}

char Character::get_character()
{
	return character;
}