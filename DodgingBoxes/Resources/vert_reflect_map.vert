#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_multiview: enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
	mat4 view;
	mat4 proj;
	int light_index;
} ubo_model;

layout(binding = 1) uniform UniformBufferLights {
	mat4 view[6];
    mat4 proj;
} ubo_reflector;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out int outLightIndex;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outCameraPos;


void main() {
	vec4 pos = ubo_model.model * vec4(inPosition, 1.0);
	outPosition = pos.xyz;
	outNormal = inNormal;
	outLightIndex = ubo_model.light_index;
	mat4 invView = inverse(ubo_reflector.view[gl_ViewIndex]);
	outCameraPos = inverse(ubo_reflector.view[gl_ViewIndex])[3].xyz;

	gl_Position = ubo_reflector.proj * ubo_reflector.view[gl_ViewIndex] * pos;
}