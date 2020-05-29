#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 fragTexCoord;

void main() {
	vec4 tex_value = texture(texSampler, fragTexCoord);

	// Outline with smoothstep antialiasing:
	float white = step(0.5, tex_value.a) + smoothstep(0.4, 0.5, tex_value.a);
	float black = step(0.4, tex_value.a) + smoothstep(0.3, 0.4, tex_value.a) - smoothstep(0.4, 0.5, tex_value.a);

	outColor = vec4(1.0, 1.0, 1.0, 1.0) * white + vec4(0.0, 0.0, 0.0, 1.0) * black;
}