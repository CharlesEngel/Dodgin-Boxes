#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec3 outPosition;
layout(location = 1) flat out vec3 outNormal;
layout(location = 2) out vec3 outModelPos;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

void main() 
{
	outModelPos = inPosition;
	outNormal = inNormal;
	outPosition = (ubo.model * vec4(inPosition, 1.0)).xyz;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}