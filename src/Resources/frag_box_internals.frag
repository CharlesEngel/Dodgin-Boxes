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
layout(location = 2) flat in vec3 inCameraPos;
layout(location = 3) in flat int inViewInd;
layout(location = 4) in flat vec3 inCenterPos;
layout(location = 5) in flat vec3 inNormal;

vec3 getBias(int viewInd)
{
	vec3 biases[6];
	biases[0] = vec3(-0.15, 0.0, 0.0);
	biases[1] = vec3(0.15, 0.0, 0.0);
	biases[2] = vec3(0.0, -0.15, 0.0);
	biases[3] = vec3(0.0, 0.15, 0.0);
	biases[4] = vec3(0.0, 0.0, -0.15);
	biases[5] = vec3(0.0, 0.0, 0.15);

	return biases[viewInd];
}

vec3 intersectWithBounds(vec3 origin, vec3 dir)
{
	float xLength = (sign(dir.x) * 0.15 - origin.x) / dir.x;
	float yLength = (sign(dir.y) * 0.15 - origin.y) / dir.y;
	float zLength = (sign(dir.z) * 0.15 - origin.z) / dir.z;

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

void main() {
	float ambient_intensity = 0.9;
	vec3 color = vec3(1.0, 1.0, 1.0);
	vec3 finalColor = vec3(0.0, 0.0, 0.0);

	vec3 dir = normalize(inPosition - vec3(0.0, 0.0, 2.0));
	vec3 origin = inPosition - (inCenterPos + getBias(inViewInd));
	vec3 endPoint = intersectWithBounds(origin, dir);
	float step_size = distance(origin, endPoint) / 10.0;
	vec3 pos = inPosition;
	float maxLightDist = 0.3;

	for (int i = 0; i < 10; i++)
	{
		vec3 normal_light_vector = normalize(lights.location[lightIndex] - pos);
		float dist = distance(pos, lights.location[lightIndex]);
		float falloff = pow(clamp(1.0 - pow(dist / maxLightDist, 4.0), 0.0, 1.0), 2.0);

		finalColor += color * lights.color[lightIndex] * falloff * (0.5 / pow(2.0, i));
		pos += dir * step_size;
	}

	/*float dist_val = clamp((distance(inPosition, lights.location[lightIndex]) - 0.15) / 0.06213, 0.0, 1.0);

	outColor = vec4(lights.color[lightIndex] * (1.0 - dist_val) + color * dist_val, 1.0);*/
	outColor = vec4(finalColor, 1.0);
}