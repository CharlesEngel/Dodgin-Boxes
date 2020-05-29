#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 inPosition;

void main() {
	outColor = vec4(0.84, 0.67, 0.23, 1.0);
}