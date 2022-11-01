#version 450

layout(location = 0) out vec4 outColour;
layout(location = 1) in vec2 fragTex;


void main()
{
	vec4 primaryColour = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 secondaryColour = vec4(1.0, 1.0, 1.0, 1.0);

	uint size = 8;
	vec2 pos = floor((fragTex * size));
	float mask = mod(pos.x + mod(pos.y, 2.0), 2.0);
	outColour = mix(primaryColour, secondaryColour, mask);
}