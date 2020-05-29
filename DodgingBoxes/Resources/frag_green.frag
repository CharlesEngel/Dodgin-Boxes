#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(0.14, 0.87, 0.21, 1.0);
}