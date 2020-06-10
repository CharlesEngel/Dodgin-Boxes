#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform LightObject {
	vec3 location[14];
	vec3 color[14];
	float intensity[14];
	float max_distance[14];
	int in_use[14];
} lights;

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 inPosition;
layout(location = 1) flat in int lightIndex;

void main() {
	float ambient_intensity = 0.9;
	vec3 ambient_color = ambient_intensity * vec3(0.23, 0.11, 0.96);

	vec3 diffuse_color[14];

	for (int i = 0; i < 14; i++)
	{
		float dist = length(inPosition - lights.location[i]);
		float falloff = pow(clamp(1.0 - pow(dist / lights.max_distance[i], 4.0), 0.0, 1.0), 2.0);
		diffuse_color[i] = step(1.0, lights.in_use[i]) * falloff * lights.intensity[i] * lights.color[i];
	}

	diffuse_color[lightIndex] = vec3(0.0, 0.0, 0.0);

	outColor = vec4(ambient_color + diffuse_color[0] + diffuse_color[1] + diffuse_color[2] + diffuse_color[3] + diffuse_color[4] + diffuse_color[5] + diffuse_color[6] + diffuse_color[7] + diffuse_color[8] + diffuse_color[9] + diffuse_color[10] + diffuse_color[11] + diffuse_color[12] + diffuse_color[13], 1.0);
}