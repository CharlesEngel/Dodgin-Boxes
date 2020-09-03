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

	const float numSteps = 15.0;

	float distToEnd = distance(origin, end)/*depth*/;
	float stepSize = distToEnd / numSteps;
	vec3 dir = normalize(end - origin);
	vec3 pos = origin;

	float opacity = (1.0 - 1.0 / (exp(distToEnd * 0.019))) /*0.0*/;

	vec3 color = colorLoad.rgb;

	vec3 currentPos = pos;

	vec3 lightValue = vec3(1.0, 1.0, 1.0) * numSteps;

	float falloffs[14];

	for (int i = 0; i < 14; i++)
	{
		float dist_top = length(origin - lights.location[i]);
		float dist_bottom = length(end - lights.location[i]);

		float falloff_top = clamp(1.0 - dist_top / lights.max_distance[i], 0.0, 1.0);
		float falloff_bottom = clamp(1.0 - dist_bottom / lights.max_distance[i], 0.0, 1.0);

		falloffs[i] = (falloff_top + falloff_bottom) / 2.0;
	}

	for (int i = 0; i < int(numSteps); i++)
	{
		//float opacity = 1.0 - depth_values[1][i]/*(1.0 - 1.0 / (exp(distance(pos, origin) * 0.01)))*/ /** step(-2.5, pos.z)*/;

		float depth_values[14];
		depth_values[0] = texture(depthMapSampler0, vec4(pos - lights.location[0], VectorToDepth(lights.location[0] - pos)));
		depth_values[1] = texture(depthMapSampler1, vec4(pos - lights.location[1], VectorToDepth(lights.location[1] - pos)));
		depth_values[2] = texture(depthMapSampler2, vec4(pos - lights.location[2], VectorToDepth(lights.location[2] - pos)));
		depth_values[3] = texture(depthMapSampler3, vec4(pos - lights.location[3], VectorToDepth(lights.location[3] - pos)));
		depth_values[4] = texture(depthMapSampler4, vec4(pos - lights.location[4], VectorToDepth(lights.location[4] - pos)));
		depth_values[5] = texture(depthMapSampler5, vec4(pos - lights.location[5], VectorToDepth(lights.location[5] - pos)));
		depth_values[6] = texture(depthMapSampler6, vec4(pos - lights.location[6], VectorToDepth(lights.location[6] - pos)));
		depth_values[7] = texture(depthMapSampler7, vec4(pos - lights.location[7], VectorToDepth(lights.location[7] - pos)));
		depth_values[8] = texture(depthMapSampler8, vec4(pos - lights.location[8], VectorToDepth(lights.location[8] - pos)));
		depth_values[9] = texture(depthMapSampler9, vec4(pos - lights.location[9], VectorToDepth(lights.location[9] - pos)));
		depth_values[10] = texture(depthMapSampler10, vec4(pos - lights.location[10], VectorToDepth(lights.location[10] - pos)));
		depth_values[11] = texture(depthMapSampler11, vec4(pos - lights.location[11], VectorToDepth(lights.location[11] - pos)));
		depth_values[12] = texture(depthMapSampler12, vec4(pos - lights.location[12], VectorToDepth(lights.location[12] - pos)));
		depth_values[13] = texture(depthMapSampler13, vec4(pos - lights.location[13], VectorToDepth(lights.location[13] - pos)));

		for (int j = 0; j < 14; j++)
		{
			lightValue += depth_values[j] * step(1.0, lights.in_use[j]) * lights.color[j] * lights.intensity[j] * falloffs[j];
		}

		pos += dir * stepSize * vec3(0.7, 0.7, 0.7);
	}

	lightValue *= stepSize;

	clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

	outColor = vec4(mix(color, lightValue, opacity), 1.0);
}