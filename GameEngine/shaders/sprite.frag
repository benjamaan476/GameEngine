#version 450

layout(location = 0) out vec4 outColour;

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec2 fragTex;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
	outColour = vec4(1, 0, 0, 1);
	//outColour = vec4(fragColour * texture(texSampler, fragTex).rgb, 1.0);
}