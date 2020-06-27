#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_multiview: enable

layout(binding = 0) uniform UniformBufferLights {
	mat4 model[6];
	mat4 view[6];
    mat4 proj;
	int lightIndex;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out int outLightIndex;
layout(location = 2) out vec3 outCameraPos;
layout(location = 3) out int outViewInd;
layout(location = 4) out vec3 outCenterPos;
layout(location = 5) out vec3 outNormal;


void main() {
	vec4 pos = ubo.model[gl_ViewIndex] * vec4(inPosition, 1.0);
	outPosition = pos.xyz;
	outLightIndex = ubo.lightIndex;
	mat4 invView = inverse(ubo.view[gl_ViewIndex]);
	outCameraPos = inverse(ubo.view[gl_ViewIndex])[3].xyz;
	outCenterPos = (ubo.model[gl_ViewIndex] * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
	outViewInd = gl_ViewIndex;
	outNormal = inNormal;

	gl_Position = ubo.proj * ubo.view[gl_ViewIndex] * pos;
}