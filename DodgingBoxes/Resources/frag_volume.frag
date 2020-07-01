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

layout(input_attachment_index = 0, binding = 16) uniform subpassInput inColor;
layout(input_attachment_index = 1, binding = 17) uniform subpassInput inDepth;

layout(location = 0) in vec3 inPosition;

float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    const float f = 2.0;
    const float n = 0.001;

	return clamp(2.0 * (1/LocalZcomp - 1/n) / (1/f - 1/n) - 1.0, 0.0, 1.0);
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
	vec3 albedo = vec3(1.0, 1.0, 1.0);
	float ambient = /*vec3(0.54, 0.36, 0.2)*/ 1.0;

	float depth = depthLinear(subpassLoad(inDepth).r) / 2.5;
	vec3 end = vec3(inPosition.xy / 2.071, depth);
	vec3 origin = vec3(inPosition.xy / 2.071, 2.0);

	float numSteps = 10.0;

	float distToEnd = distance(origin, end);
	float stepSize = distToEnd / numSteps;
	vec3 dir = normalize(end - origin);
	vec3 pos = origin;
	float overallOpacity = 1.0 / exp(distToEnd * 0.7);

	float depth_map_value[14];

	vec3 color = /*albedo * ambient * stepSize*/ vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < int(numSteps); i++)
	{
		depth_map_value[0] = texture(depthMapSampler0, vec4(pos - lights.location[0], VectorToDepth(lights.location[0] - pos)));
		depth_map_value[1] = texture(depthMapSampler1, vec4(pos - lights.location[1], VectorToDepth(lights.location[1] - pos)));
		depth_map_value[2] = texture(depthMapSampler2, vec4(pos - lights.location[2], VectorToDepth(lights.location[2] - pos)));
		depth_map_value[3] = texture(depthMapSampler3, vec4(normalize(pos - lights.location[3]), VectorToDepth(lights.location[3] - pos)));
		depth_map_value[4] = texture(depthMapSampler4, vec4(normalize(pos - lights.location[4]), VectorToDepth(lights.location[4] - pos)));
		depth_map_value[5] = texture(depthMapSampler5, vec4(normalize(pos - lights.location[5]), VectorToDepth(lights.location[5] - pos)));
		depth_map_value[6] = texture(depthMapSampler6, vec4(normalize(pos - lights.location[6]), VectorToDepth(lights.location[6] - pos)));
		depth_map_value[7] = texture(depthMapSampler7, vec4(normalize(pos - lights.location[7]), VectorToDepth(lights.location[7] - pos)));
		depth_map_value[8] = texture(depthMapSampler8, vec4(normalize(pos - lights.location[8]), VectorToDepth(lights.location[8] - pos)));
		depth_map_value[9] = texture(depthMapSampler9, vec4(normalize(pos - lights.location[9]), VectorToDepth(lights.location[9] - pos)));
		depth_map_value[10] = texture(depthMapSampler10, vec4(normalize(pos - lights.location[10]), VectorToDepth(lights.location[10] - pos)));
		depth_map_value[11] = texture(depthMapSampler11, vec4(normalize(pos - lights.location[11]), VectorToDepth(lights.location[11] - pos)));
		depth_map_value[12] = texture(depthMapSampler12, vec4(normalize(pos - lights.location[12]), VectorToDepth(lights.location[12] - pos)));
		depth_map_value[13] = texture(depthMapSampler13, vec4(normalize(pos - lights.location[13]), VectorToDepth(lights.location[13] - pos)));

		float opacity = 1.0 / exp(distance(pos, origin) * 0.7);

		vec3 lightValue = vec3(0.2, 0.2, 0.2);
		for (int j = 0; j < 14; j++)
		{
			vec3 normal_light_vector = normalize(lights.location[j] - pos);
			float dist = length(pos - lights.location[j]);
			float falloff = pow(clamp(1.0 - pow(dist / lights.max_distance[j], 4.0), 0.0, 1.0), 2.0);

			lightValue += depth_map_value[j] * step(1.0, lights.in_use[j]) * lights.color[j] * lights.intensity[j] * albedo * falloff / 6.0;
		}

		color = (color * (1.0 - opacity) + lightValue * opacity)/* * stepSize*/;

		pos += dir * stepSize;
	}

	clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

	float alpha = overallOpacity;

	outColor = vec4(color * alpha + subpassLoad(inColor).rgb * (1.0 - alpha), 1.0);
}