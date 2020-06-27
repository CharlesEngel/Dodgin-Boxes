#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform LightObject {
	vec3 location[14];
	vec3 color[14];
	float intensity[14];
	float max_distance[14];
	int in_use[14];
} lights;

layout(binding = 2) uniform samplerCubeShadow depthMapSampler0;
layout(binding = 3) uniform samplerCubeShadow depthMapSampler1;
layout(binding = 4) uniform samplerCubeShadow depthMapSampler2;
layout(binding = 5) uniform samplerCubeShadow depthMapSampler3;
layout(binding = 6) uniform samplerCubeShadow depthMapSampler4;
layout(binding = 7) uniform samplerCubeShadow depthMapSampler5;
layout(binding = 8) uniform samplerCubeShadow depthMapSampler6;
layout(binding = 9) uniform samplerCubeShadow depthMapSampler7;
layout(binding = 10) uniform samplerCubeShadow depthMapSampler8;
layout(binding = 11) uniform samplerCubeShadow depthMapSampler9;
layout(binding = 12) uniform samplerCubeShadow depthMapSampler10;
layout(binding = 13) uniform samplerCubeShadow depthMapSampler11;
layout(binding = 14) uniform samplerCubeShadow depthMapSampler12;
layout(binding = 15) uniform samplerCubeShadow depthMapSampler13;
layout(binding = 16) uniform samplerCube reflectMapSampler;
layout(binding = 17) uniform samplerCube boxInternalsSampler;
layout(binding = 18) uniform sampler2D roughnessSamplerTop;
layout(binding = 19) uniform sampler2D roughnessSamplerHoriz;
layout(binding = 20) uniform sampler2D roughnessSamplerVert;

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 inPosition;
layout(location = 1) flat in int lightIndex;
layout(location = 2) flat in vec3 normal;
layout(location = 3) flat in vec3 inCameraPos;
layout(location = 4) in vec3 inModelPos;
layout(location = 5) flat in vec3 inCenterPos;

float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 2.0;
    const float n = 0.001;

	return clamp(2.0 * (1/LocalZcomp - 1/n) / (1/f - 1/n) - 1.0001, 0.0, 1.0);
}

vec3 IntersectWithRoom(vec3 origin, vec3 dir) {
	float xLength = (sign(dir.x) * 1.0 - origin.x) / dir.x;
	float yLength = (sign(dir.y) * 1.0 - origin.y) / dir.y;
	float zLength = (sign(dir.z) * 0.5 - origin.z) / dir.z;

	vec3 xVec = vec3(xLength * dir.x, xLength * dir.y, xLength * dir.z);
	vec3 yVec = vec3(yLength * dir.x, yLength * dir.y, yLength * dir.z);
	vec3 zVec = vec3(zLength * dir.x, zLength * dir.y, zLength * dir.z);

	float distX = length(xVec);
	float distY = length(yVec);
	float distZ = length(zVec);

	float minDist = min(distX, min(distY, distZ));

	if (minDist == distX)
	{
		return origin + xVec;
	}
	else if (minDist == distY)
	{
		return origin + yVec;
	}

	return origin + zVec;
}

float g1(float dotNV, float k) {
	return 1.0 / (dotNV * (1.0 - k) + k);
}

float ggx(vec3 v, vec3 l, vec3 n, float ind, float roughness) {
	vec3 h = normalize(v + l);
	float alpha = roughness * roughness;
	float alphaSqr = alpha * alpha;
	float c = dot(v, h);
	float g = sqrt(pow(c, 2.0) + pow(ind, 2.0) - 1.0);

	float F = 0.5 * (pow(g-c,2.0)/pow(g+c,2.0)) * (1.0 + (pow(c*(g+c)-1.0, 2.0)/pow(c*(g-c)+1.0, 2.0)));
	float denominator = clamp(dot(n, h), 0.0, 1.0) * clamp(dot(n, h), 0.0, 1.0) * (alphaSqr - 1.0) + 1.0;
	float D = alphaSqr / (3.141592 * denominator * denominator);
	float k = alpha/2.0;
	float visibility = g1(clamp(dot(n, l), 0.0, 1.0), k) * g1(clamp(dot(n, v), 0.0, 1.0), k);
	return clamp(dot(n, l), 0.0, 1.0) * F * D * visibility;
}

float getRoughness(vec3 vector)
{
	float roughness;

	float xSqr = vector.x * vector.x;
	float ySqr = vector.y * vector.y;
	float zSqr = vector.z * vector.z;

	float maxSqr = max(xSqr, max(ySqr, zSqr));

	if (maxSqr == ySqr)
	{
		roughness = texture(roughnessSamplerVert, vec2(vector.x, vector.z)).r;
	}
	else if (maxSqr == xSqr)
	{
		roughness = texture(roughnessSamplerHoriz, vec2(vector.y, vector.z)).r;
	}
	else
	{
		roughness = texture(roughnessSamplerTop, vec2(vector.x, vector.y)).r;
	}

	return pow((roughness * 1.055), 1.0 / 2.4);
}

void main() {
	/*float ambient_intensity = 0.9;
	vec3 ambient_color = ambient_intensity * vec3(0.23, 0.11, 0.96)*/;

	vec3 diffuse_color[14];

	float depth_map_value_0 = texture(depthMapSampler0, vec4(inPosition - lights.location[0], VectorToDepth(lights.location[0] - inPosition)), 0.0);
	float depth_map_value_1 = texture(depthMapSampler1, vec4(inPosition - lights.location[1], VectorToDepth(lights.location[1] - inPosition)), 0.0);
	float depth_map_value_2 = texture(depthMapSampler2, vec4(inPosition - lights.location[2], VectorToDepth(lights.location[2] - inPosition)), 0.0);
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

	vec3 rNorm = reflect(normalize(inPosition - inCameraPos), normal);
	vec3 intersect = IntersectWithRoom(inPosition, rNorm);
	float roughness = getRoughness(inModelPos);
	vec4 reflect_value = textureLod(reflectMapSampler, intersect - (inCenterPos), 10.0 * roughness);
	vec3 ambient_color = textureLod(boxInternalsSampler, inModelPos, 3.0 + 7.0 * roughness).xyz;

	for (int i = 0; i < 14; i++)
	{
		vec3 normal_light_vector = normalize(lights.location[i] - inPosition);
		float dist = length(inPosition - lights.location[i]);
		float falloff = pow(clamp(1.0 - pow(dist / lights.max_distance[i], 4.0), 0.0, 1.0), 2.0) * clamp(dot(normal, normal_light_vector), 0.0, 1.0);
		diffuse_color[i] = step(1.0, lights.in_use[i]) * vec3(1.0, 1.0, 1.0) * ggx(normalize(inCameraPos - inPosition), normal_light_vector, normal, 1.3, roughness) * falloff;
	}

	diffuse_color[lightIndex] = vec3(0.0, 0.0, 0.0);

	vec4 color = vec4(ambient_color + depth_map_value_0 * diffuse_color[0] + depth_map_value_1 * diffuse_color[1] + depth_map_value_2 * diffuse_color[2] + depth_map_value_3 * diffuse_color[3] + depth_map_value_4 * diffuse_color[4] + depth_map_value_5 * diffuse_color[5] + depth_map_value_6 * diffuse_color[6] + depth_map_value_7 * diffuse_color[7] + depth_map_value_8 * diffuse_color[8] + depth_map_value_9 * diffuse_color[9] + depth_map_value_10 * diffuse_color[10] + depth_map_value_11 * diffuse_color[11] + depth_map_value_12 * diffuse_color[12] + depth_map_value_13 * diffuse_color[13], 1.0);
	outColor = (reflect_value + color);
}