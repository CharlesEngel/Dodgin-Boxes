#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 inModelPos;
layout(location = 1) in flat float time;

void main() {
	vec2 boundsDim = vec2(0.5, 0.3);

	float distance = length(max(abs(inModelPos.xy) - boundsDim, 0.0)) + 0.1 * sin(inModelPos.x + time);

	outColor = vec4(((inModelPos.x + 0.5) * vec3(0.21, 0.013, 0.988) + (1.0 - (inModelPos.x + 0.5)) * vec3(0.983, 0.21, 0.14)) * (1.0 - smoothstep(0.0, 0.08, distance)), 1.0);
}