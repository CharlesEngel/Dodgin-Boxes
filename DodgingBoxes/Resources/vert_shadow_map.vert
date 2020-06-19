#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_multiview: enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
	mat4 view;
	mat4 proj;
	int light_index;
} ubo_model;

layout(binding = 1) uniform UniformBufferLights {
	mat4 view[6];
    mat4 proj[6];
	int light_index;
} ubo_light;

layout(location = 0) in vec3 inPosition;


void main() {
	gl_Position = ubo_light.proj[gl_ViewIndex] * ubo_light.view[gl_ViewIndex] * ubo_model.model * vec4(inPosition, 1.0);
}