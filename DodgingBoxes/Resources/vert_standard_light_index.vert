#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	int light_index;
} ubo;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out int lightIndex;

layout(location = 0) in vec3 inPosition;

void main() {
	lightIndex = ubo.light_index;
	outPosition = (ubo.model * vec4(inPosition, 1.0)).xyz;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}