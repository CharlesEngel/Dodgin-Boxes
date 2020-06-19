#include "Floor.h"

#include <glm/gtc/matrix_transform.hpp>

Floor::Floor(Renderer *renderer)
{
	this->renderer = renderer;
	//transform = glm::scale(glm::mat4(1), glm::vec3(100.0, 100.0, 100.0));
	transform = glm::scale(glm::translate(glm::mat4(1), glm::vec3(0.0, 0.0, -1.0)), glm::vec3(100.0, 100.0, 1.0));

	UniformBufferParameters uniform_parameters = {};
	uniform_parameters.size = sizeof(FloorUniform);

	uniform_buffer = get_uniform_buffer(*renderer, uniform_parameters);

	InstanceParameters instance_parameters = {};
	instance_parameters.material = MATERIAL_RED_SQUARE;
	instance_parameters.uniform_buffers = { uniform_buffer };

	instance = create_instance(*renderer, instance_parameters);
}

Floor::~Floor()
{
	if (renderer->device.device != VK_NULL_HANDLE)
	{
		free_uniform_buffer(*renderer, uniform_buffer);
		free_instance(*renderer, instance);
	}
}

void Floor::update(double time)
{

}

std::vector<Rectangle *> Floor::get_collider()
{
	return {};
}

void Floor::submit_for_rendering(glm::mat4 view, glm::mat4 proj, float width, float height) const
{
	FloorUniform buffer_data = {};
	buffer_data.model = transform;
	buffer_data.proj = proj;
	buffer_data.view = view;
	buffer_data.light_index = -1;

	UniformBufferUpdateParameters update_parameters = {};
	update_parameters.buffer_name = uniform_buffer;
	update_parameters.data = &buffer_data;

	update_uniform_buffer(*renderer, update_parameters);

	InstanceSubmitParameters submit_parameters = {};
	submit_parameters.instance_name = instance;

	submit_instance(*renderer, submit_parameters);
}

void Floor::handle_external_collisions(const Rectangle *collider, const GameObject *other)
{

}