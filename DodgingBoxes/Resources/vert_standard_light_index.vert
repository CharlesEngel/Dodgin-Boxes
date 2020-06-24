#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	int light_index;
} ubo;

layout(location = 0) out vec3 outPosition;
layout(location = 1) flat out int lightIndex;
layout(location = 2) flat out vec3 outNormal;
layout(location = 3) flat out vec3 outCameraPos;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

void main() {
	outNormal = inNormal;
	lightIndex = ubo.light_index;
	vec4 pos = ubo.model * vec4(inPosition, 1.0);
	outCameraPos = inverse(ubo.view)[3].xyz;

	outPosition = pos.xyz;
    gl_Position = ubo.proj * ubo.view * pos;
}