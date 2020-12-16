#version 450
#extension GL_ARB_separate_shader_objects : enable

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

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 inPosition;
layout(location = 1) flat in int lightIndex;
layout(location = 2) flat in vec3 normal;
layout(location = 3) flat in vec3 inCameraPos;

float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 2.0;
    const float n = 0.001;

	return clamp(2.0 * (1/LocalZcomp - 1/n) / (1/f - 1/n) - 1.0001, 0.0, 1.0);
}

float fresnel(vec3 v, vec3 l, float ind) {
	vec3 h = normalize(v + l);
	float c = dot(v, h);
	float g = sqrt(pow(c, 2.0) + pow(ind, 2.0) - 1.0);

	float F = 0.5 * (pow(g-c,2.0)/pow(g+c,2.0)) * (1.0 + (pow(c*(g+c)-1.0, 2.0)/pow(c*(g-c)+1.0, 2.0)));
	return F;
}

vec3 diffBRDF(vec3 albedo, vec3 l, vec3 v, vec3 n, float roughness, float F0)
{
	vec3 h = (l + v) / 2.0;
	float fmulti = 0.3641 * roughness;
	float kfacing = 0.5 + 0.5 * dot(l, v);
	float frough = kfacing * (0.9 - 0.4 * kfacing) * ((0.5 + dot(n, h)) / dot(n, h));
	float fsmooth = (21.0 / 20.0) * (1.0 - F0) * (1.0 - pow(1.0 - dot(n, l), 5.0)) * (1.0 - pow(1.0 - dot(n, v), 5.0));
	vec3 fdiff = step(dot(-n, l), 0.0) * step(dot(-n, v), 0.0) * (albedo / 3.141592) * ((1.0 - roughness) * fsmooth + roughness * frough + albedo * fmulti);
	return fdiff;
}

void main() {
	vec3 camera = normalize(inCameraPos - inPosition);

	vec3 diffuse_color[14];
	vec3 ambient_color = vec3(0.42, 0.12, 0.06);

	float depth_map_value[14];

	depth_map_value[0] = texture(depthMapSampler0, vec4(inPosition - lights.location[0], VectorToDepth(lights.location[0] - inPosition)), 0.0);
	depth_map_value[1] = texture(depthMapSampler1, vec4(inPosition - lights.location[1], VectorToDepth(lights.location[1] - inPosition)), 0.0);
	depth_map_value[2] = texture(depthMapSampler2, vec4(inPosition - lights.location[2], VectorToDepth(lights.location[2] - inPosition)), 0.0);
	depth_map_value[3] = texture(depthMapSampler3, vec4(normalize(inPosition - lights.location[3]), VectorToDepth(lights.location[3] - inPosition)), 0.0);
	depth_map_value[4] = texture(depthMapSampler4, vec4(normalize(inPosition - lights.location[4]), VectorToDepth(lights.location[4] - inPosition)), 0.0);
	depth_map_value[5] = texture(depthMapSampler5, vec4(normalize(inPosition - lights.location[5]), VectorToDepth(lights.location[5] - inPosition)), 0.0);
	depth_map_value[6] = texture(depthMapSampler6, vec4(normalize(inPosition - lights.location[6]), VectorToDepth(lights.location[6] - inPosition)), 0.0);
	depth_map_value[7] = texture(depthMapSampler7, vec4(normalize(inPosition - lights.location[7]), VectorToDepth(lights.location[7] - inPosition)), 0.0);
	depth_map_value[8] = texture(depthMapSampler8, vec4(normalize(inPosition - lights.location[8]), VectorToDepth(lights.location[8] - inPosition)), 0.0);
	depth_map_value[9] = texture(depthMapSampler9, vec4(normalize(inPosition - lights.location[9]), VectorToDepth(lights.location[9] - inPosition)), 0.0);
	depth_map_value[10] = texture(depthMapSampler10, vec4(normalize(inPosition - lights.location[10]), VectorToDepth(lights.location[10] - inPosition)), 0.0);
	depth_map_value[11] = texture(depthMapSampler11, vec4(normalize(inPosition - lights.location[11]), VectorToDepth(lights.location[11] - inPosition)), 0.0);
	depth_map_value[12] = texture(depthMapSampler12, vec4(normalize(inPosition - lights.location[12]), VectorToDepth(lights.location[12] - inPosition)), 0.0);
	depth_map_value[13] = texture(depthMapSampler13, vec4(normalize(inPosition - lights.location[13]), VectorToDepth(lights.location[13] - inPosition)), 0.0);

	depth_map_value[lightIndex] = 1.0;

	for (int i = 0; i < 14; i++)
	{
		vec3 normal_light_vector = normalize(lights.location[i] - inPosition);
		vec3 normal_view_vector = normalize(inCameraPos - inPosition);
		float dist = length(inPosition - lights.location[i]);
		float falloff = pow(clamp(1.0 - pow(dist / lights.max_distance[i], 4.0), 0.0, 1.0), 2.0) * clamp(dot(normal, normal_light_vector), 0.0, 1.0);

		float fresnel = fresnel(normal_view_vector, normal_light_vector, 1.3);
		vec3 diffuse = diffBRDF(ambient_color, normal_light_vector, normal_view_vector, normal, 1.0, fresnel);

		//diffuse_color[i] = 30.0 * step(1.0, lights.in_use[i]) * falloff * lights.intensity[i] * diffuse * lights.color[i];

		vec3 LTLight = (normalize((lights.location[i] - inPosition)) + 0.2 * normal);
		float LTDot = pow(clamp(dot(camera, -LTLight), 0.0, 1.0), 12.0) * 2.0;
		float LTAttenuation = 1.0 / dot(lights.location[i] - inPosition, lights.location[i] - inPosition);
		float LT = LTAttenuation * (LTDot + 0.2) * 0.36;
		diffuse_color[i] = 10.0 * step(1.0, lights.in_use[i]) * falloff * lights.intensity[i] * diffuse * lights.color[i] + lights.color[i] * vec3(0.42, 0.12, 0.06) * LT;
	}

	outColor = vec4(depth_map_value[0] * diffuse_color[0] + depth_map_value[1] * diffuse_color[1] + depth_map_value[2] * diffuse_color[2] + depth_map_value[3] * diffuse_color[3] + depth_map_value[4] * diffuse_color[4] + depth_map_value[5] * diffuse_color[5] + depth_map_value[6] * diffuse_color[6] + depth_map_value[7] * diffuse_color[7] + depth_map_value[8] * diffuse_color[8] + depth_map_value[9] * diffuse_color[9] + depth_map_value[10] * diffuse_color[10] + depth_map_value[11] * diffuse_color[11] + depth_map_value[12] * diffuse_color[12] + depth_map_value[13] * diffuse_color[13], 1.0);
}