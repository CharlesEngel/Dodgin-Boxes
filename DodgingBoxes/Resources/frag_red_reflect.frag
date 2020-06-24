#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform LightObject {
	vec3 location[14];
	vec3 color[14];
	float intensity[14];
	float max_distance[14];
	int in_use[14];
} lights;

layout(binding = 3) uniform samplerCubeShadow depthMapSampler0;
layout(binding = 4) uniform samplerCubeShadow depthMapSampler1;
layout(binding = 5) uniform samplerCubeShadow depthMapSampler2;
layout(binding = 6) uniform samplerCubeShadow depthMapSampler3;
layout(binding = 7) uniform samplerCubeShadow depthMapSampler4;
layout(binding = 8) uniform samplerCubeShadow depthMapSampler5;
layout(binding = 9) uniform samplerCubeShadow depthMapSampler6;
layout(binding = 10) uniform samplerCubeShadow depthMapSampler7;
layout(binding = 11) uniform samplerCubeShadow depthMapSampler8;
layout(binding = 12) uniform samplerCubeShadow depthMapSampler9;
layout(binding = 13) uniform samplerCubeShadow depthMapSampler10;
layout(binding = 14) uniform samplerCubeShadow depthMapSampler11;
layout(binding = 15) uniform samplerCubeShadow depthMapSampler12;
layout(binding = 16) uniform samplerCubeShadow depthMapSampler13;

float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 2.0;
    const float n = 0.001;

	return clamp(2.0 * (1/LocalZcomp - 1/n) / (1/f - 1/n) - 1.0002, 0.0, 1.0);
}


layout(location = 0) in vec3 inPosition;
layout(location = 1) flat in int lightIndex;
layout(location = 2) flat in vec3 normal;

void main() {
	float ambient_intensity = 0.3;
	vec3 ambient_color = ambient_intensity * vec3(0.4, 0.1, 0.12);

	vec3 diffuse_color[14];

	float depth_map_value_0 = texture(depthMapSampler0, vec4(inPosition - lights.location[0], VectorToDepth(lights.location[0] - inPosition) + 0.0001));
	float depth_map_value_1 = texture(depthMapSampler1, vec4(inPosition - lights.location[1], VectorToDepth(lights.location[1] - inPosition)));
	float depth_map_value_2 = texture(depthMapSampler2, vec4(inPosition - lights.location[2], VectorToDepth(lights.location[2] - inPosition)));
	float depth_map_value_3 = texture(depthMapSampler3, vec4(normalize(inPosition - lights.location[3]), VectorToDepth(lights.location[3] - inPosition)), 0.0);
	float depth_map_value_4 = texture(depthMapSampler4, vec4(normalize(inPosition - lights.location[4]), VectorToDepth(lights.location[4] - inPosition)), 0.0);
	float depth_map_value_5 = texture(depthMapSampler5, vec4(normalize(inPosition - lights.location[5]), VectorToDepth(lights.location[5] - inPosition)), 0.0);
	float depth_map_value_6 = texture(depthMapSampler6, vec4(normalize(inPosition - lights.location[6]), VectorToDepth(lights.location[6] - inPosition)), 0.0);
	float depth_map_value_7 = texture(depthMapSampler7, vec4(normalize(inPosition - lights.location[7]), VectorToDepth(lights.location[7] - inPosition)), 0.0);
	float depth_map_value_8 = texture(depthMapSampler8, vec4(normalize(inPosition - lights.location[8]), VectorToDepth(lights.location[8] - inPosition)), 0.0);
	float depth_map_value_9 = texture(depthMapSampler9, vec4(normalize(inPosition - lights.location[9]), VectorToDepth(lights.location[9] - inPosition)), 0.0);
	float depth_map_value_10 = texture(depthMapSampler10, vec4(normalize(inPosition - lights.location[10]), VectorToDepth(lights.location[10] - inPosition)), 0.0);
	float depth_map_value_11 = texture(depthMapSampler11, vec4(normalize(inPosition - lights.location[11]), VectorToDepth(lights.location[11] - inPosition)), 0.0);
	float depth_map_value_12 = texture(depthMapSampler12, vec4(normalize(inPosition - lights.location[12]), VectorToDepth(lights.location[12] - inPosition)), 0.0);
	float depth_map_value_13 = texture(depthMapSampler13, vec4(normalize(inPosition - lights.location[13]), VectorToDepth(lights.location[13] - inPosition)), 0.0);

	for (int i = 0; i < 14; i++)
	{
		vec3 normal_light_vector = normalize(lights.location[i] - inPosition);
		float dist = length(inPosition - lights.location[i]);
		float falloff = pow(clamp(1.0 - pow(dist / lights.max_distance[i], 4.0), 0.0, 1.0), 2.0) * clamp(dot(normal, normal_light_vector), 0.0, 1.0);

		diffuse_color[i] = step(1.0, lights.in_use[i]) * falloff * lights.intensity[i] * lights.color[i];
	}

	outColor = vec4(ambient_color + depth_map_value_0 * diffuse_color[0] + depth_map_value_1 * diffuse_color[1] + depth_map_value_2 * diffuse_color[2] + depth_map_value_3 * diffuse_color[3] + depth_map_value_4 * diffuse_color[4] + depth_map_value_5 * diffuse_color[5] + depth_map_value_6 * diffuse_color[6] + depth_map_value_7 * diffuse_color[7] + depth_map_value_8 * diffuse_color[8] + depth_map_value_9 * diffuse_color[9] + depth_map_value_10 * diffuse_color[10] + depth_map_value_11 * diffuse_color[11] + depth_map_value_12 * diffuse_color[12] + depth_map_value_13 * diffuse_color[13], 1.0);
}