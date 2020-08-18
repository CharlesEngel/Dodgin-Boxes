#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

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

layout(input_attachment_index = 0, binding = 16) uniform subpassInputMS inColor;
layout(input_attachment_index = 1, binding = 17) uniform subpassInputMS inDepth;

layout(location = 0) in vec3 inPosition;

float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 2.0;
    const float n = 0.001;

	return clamp(2.0 * (1/LocalZcomp - 1/n) / (1/f - 1/n) - 1.0000, 0.0, 1.0);
}

float depthLinear(float depth)
{
	float zNear = 0.1;
	float zFar = 10.0;

	depth = 2.0 * depth - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
	vec4 colorLoad = 0.25 * subpassLoad(inColor, 0) + 0.25 * subpassLoad(inColor, 1) + 0.25 * subpassLoad(inColor, 2) + 0.25 * subpassLoad(inColor, 3);
	float depth = depthLinear(subpassLoad(inDepth, 0).r) * 1.0;
	vec3 end = vec3(inPosition.xy / 2.071, 2.0-depth);
	vec3 origin = vec3(inPosition.xy / 2.071, 2.0);

	const float numSteps = 10.0;

	float distToEnd = /*distance(origin, end)*/depth;
	float stepSize = distToEnd / numSteps;
	vec3 dir = normalize(end - origin);
	vec3 pos = origin;

	float depth_map_value[14];

	vec3 color = colorLoad.rgb;

	vec3 currentPos = pos;

	float depth_values[14][int(numSteps)];

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[0][i] = texture(depthMapSampler0, vec4(currentPos - lights.location[0], VectorToDepth(lights.location[0] - currentPos)));
		depth_values[0][i+1] = depth_values[0][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[1][i] = texture(depthMapSampler1, vec4(currentPos - lights.location[1], VectorToDepth(lights.location[1] - currentPos)));
		depth_values[1][i+1] = depth_values[1][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[2][i] = texture(depthMapSampler2, vec4(currentPos - lights.location[2], VectorToDepth(lights.location[2] - currentPos)));
		depth_values[2][i+1] = depth_values[2][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[3][i] = texture(depthMapSampler3, vec4(currentPos - lights.location[3], VectorToDepth(lights.location[3] - currentPos)));
		depth_values[3][i+1] = depth_values[3][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[4][i] = texture(depthMapSampler4, vec4(currentPos - lights.location[4], VectorToDepth(lights.location[4] - currentPos)));
		depth_values[4][i+1] = depth_values[4][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[5][i] = texture(depthMapSampler5, vec4(currentPos - lights.location[5], VectorToDepth(lights.location[5] - currentPos)));
		depth_values[5][i+1] = depth_values[5][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[6][i] = texture(depthMapSampler6, vec4(currentPos - lights.location[6], VectorToDepth(lights.location[6] - currentPos)));
		depth_values[6][i+1] = depth_values[6][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[7][i] = texture(depthMapSampler7, vec4(currentPos - lights.location[7], VectorToDepth(lights.location[7] - currentPos)));
		depth_values[7][i+1] = depth_values[7][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[8][i] = texture(depthMapSampler8, vec4(currentPos - lights.location[8], VectorToDepth(lights.location[8] - currentPos)));
		depth_values[8][i+1] = depth_values[8][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[9][i] = texture(depthMapSampler9, vec4(currentPos - lights.location[9], VectorToDepth(lights.location[9] - currentPos)));
		depth_values[9][i+1] = depth_values[9][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[10][i] = texture(depthMapSampler10, vec4(currentPos - lights.location[10], VectorToDepth(lights.location[10] - currentPos)));
		depth_values[10][i+1] = depth_values[10][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[11][i] = texture(depthMapSampler11, vec4(currentPos - lights.location[11], VectorToDepth(lights.location[11] - currentPos)));
		depth_values[11][i+1] = depth_values[11][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[12][i] = texture(depthMapSampler12, vec4(currentPos - lights.location[12], VectorToDepth(lights.location[12] - currentPos)));
		depth_values[12][i+1] = depth_values[12][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i += 2)
	{
		depth_values[13][i] = texture(depthMapSampler13, vec4(currentPos - lights.location[13], VectorToDepth(lights.location[13] - currentPos)));
		depth_values[13][i+1] = depth_values[13][i];
		currentPos += dir * 2.0 * stepSize;
	}

	currentPos = pos;

	for (int i = 0; i < int(numSteps); i++)
	{
		float opacity = (1.0 - 1.0 / exp(distance(pos, end) * 0.029)) * step(-2.5, pos.z);

		vec3 lightValue = vec3(0.7, 0.7, 0.7);
		for (int j = 0; j < 14; j++)
		{
			vec3 normal_light_vector = normalize(lights.location[j] - pos);
			float dist = length(pos - lights.location[j]);
			float falloff = pow(clamp(1.0 - pow(dist / lights.max_distance[j], 4.0), 0.0, 1.0), 2.0);

			lightValue += ((depth_values[j][i] + 0.4) / 1.4) * step(1.0, lights.in_use[j]) * lights.color[j] * lights.intensity[j] * falloff;
		}

		color = (color * (1.0 - opacity) + lightValue * opacity);

		pos += dir * stepSize;
	}

	clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

	outColor = vec4(color, 1.0);
}