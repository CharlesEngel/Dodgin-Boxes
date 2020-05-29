#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
	float x;
	float y;
	float width;
	float height;
} ubo;

layout(location = 0) out vec2 outTexCoord;
//layout(location = 0) out vec2 size;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

void main() {
	outTexCoord = vec2(inTexCoord.x * ubo.width + ubo.x, inTexCoord.y * ubo.height + ubo.y);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}