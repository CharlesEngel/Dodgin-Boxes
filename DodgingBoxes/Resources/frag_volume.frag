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

	return 1.0 / ((((depth + 1) * (1/zFar - 1/zNear)) / 2.0) + 1/zNear);
}


void main()
{
	vec4 colorLoad = 0.25 * subpassLoad(inColor, 0) + 0.25 * subpassLoad(inColor, 1) + 0.25 * subpassLoad(inColor, 2) + 0.25 * subpassLoad(inColor, 3);
	float depth = depthLinear(subpassLoad(inDepth, 0).r);
	vec3 end = vec3(inPosition.xy, 2.0-depth);
	vec3 origin = vec3(0.0, 0.0, 2.0);

	const float numSteps = 10.0;

	float distToEnd = distance(origin, end)/*depth*/;
	float stepSize = distToEnd / numSteps;
	vec3 dir = normalize(end - origin);
	vec3 pos = origin;

	float opacity = (1.0 - 1.0 / (exp(distToEnd * 0.04))) /*0.0*/;

	vec3 color = colorLoad.rgb;

	vec3 lightValue = vec3(1.0, 1.0, 1.0) * numSteps;

	// Start calculating lighting and fog
	// It was broken up per light to improve cache coherency while avoiding arrays
	vec3 currentPos = pos;

	float dist_top = length(origin - lights.location[0]);
	float dist_bottom = length(end - lights.location[0]);

	float falloff_top = clamp(1.0 - dist_top / lights.max_distance[0], 0.0, 1.0);
	float falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[0], 0.0, 1.0);

	float falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler0, vec4(currentPos - lights.location[0], VectorToDepth(lights.location[0] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[0]) * lights.color[0] * lights.intensity[0] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[1]);
	dist_bottom = length(end - lights.location[1]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[1], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[1], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler1, vec4(currentPos - lights.location[1], VectorToDepth(lights.location[1] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[1]) * lights.color[1] * lights.intensity[1] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[2]);
	dist_bottom = length(end - lights.location[2]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[2], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[2], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{

		float depth_value = texture(depthMapSampler2, vec4(currentPos - lights.location[2], VectorToDepth(lights.location[2] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[2]) * lights.color[2] * lights.intensity[2] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[3]);
	dist_bottom = length(end - lights.location[3]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[3], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[3], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler3, vec4(currentPos - lights.location[3], VectorToDepth(lights.location[3] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[3]) * lights.color[3] * lights.intensity[3] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[4]);
	dist_bottom = length(end - lights.location[4]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[4], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[4], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler4, vec4(currentPos - lights.location[4], VectorToDepth(lights.location[4] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[4]) * lights.color[4] * lights.intensity[4] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[5]);
	dist_bottom = length(end - lights.location[5]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[5], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[5], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler5, vec4(currentPos - lights.location[5], VectorToDepth(lights.location[5] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[5]) * lights.color[5] * lights.intensity[5] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[6]);
	dist_bottom = length(end - lights.location[6]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[6], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[6], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler6, vec4(currentPos - lights.location[6], VectorToDepth(lights.location[6] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[6]) * lights.color[6] * lights.intensity[6] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[7]);
	dist_bottom = length(end - lights.location[7]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[7], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[7], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler7, vec4(currentPos - lights.location[7], VectorToDepth(lights.location[7] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[7]) * lights.color[7] * lights.intensity[7] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[8]);
	dist_bottom = length(end - lights.location[8]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[8], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[8], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler8, vec4(currentPos - lights.location[8], VectorToDepth(lights.location[8] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[8]) * lights.color[8] * lights.intensity[8] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[9]);
	dist_bottom = length(end - lights.location[9]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[9], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[9], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler9, vec4(currentPos - lights.location[9], VectorToDepth(lights.location[9] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[9]) * lights.color[9] * lights.intensity[9] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[10]);
	dist_bottom = length(end - lights.location[10]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[10], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[10], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler10, vec4(currentPos - lights.location[10], VectorToDepth(lights.location[10] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[10]) * lights.color[10] * lights.intensity[10] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[11]);
	dist_bottom = length(end - lights.location[11]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[11], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[11], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler11, vec4(currentPos - lights.location[11], VectorToDepth(lights.location[11] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[11]) * lights.color[11] * lights.intensity[11] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[12]);
	dist_bottom = length(end - lights.location[12]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[12], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[12], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler12, vec4(currentPos - lights.location[12], VectorToDepth(lights.location[12] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[12]) * lights.color[12] * lights.intensity[0] * falloff;
		currentPos += dir * stepSize;
	}

	currentPos = pos;

	dist_top = length(origin - lights.location[13]);
	dist_bottom = length(end - lights.location[13]);

	falloff_top = clamp(1.0 - dist_top / lights.max_distance[13], 0.0, 1.0);
	falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[13], 0.0, 1.0);

	falloff = (falloff_top + falloff_bottom) / 2.0;

	for (int i = 0; i < int(numSteps); i++)
	{
		float depth_value = texture(depthMapSampler13, vec4(currentPos - lights.location[13], VectorToDepth(lights.location[13] - currentPos)));

		lightValue += depth_value * step(1.0, lights.in_use[13]) * lights.color[13] * lights.intensity[13] * falloff;
		currentPos += dir * stepSize;
	}

	lightValue *= stepSize * vec3(0.7, 0.7, 0.7);

	clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

	outColor = vec4(mix(color, lightValue, opacity), 1.0);
}