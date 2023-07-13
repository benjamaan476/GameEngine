#version 450


layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColour;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inViewVec;
layout (location = 4) in vec3 inLightVec;

layout (location = 0) out vec4 outFragColour;

void main()
{
vec4 colour = vec4(1.0);//texture(samplerColourMap, inUV) * vec4(inColour, 1.0);

	vec3 N = normalize(inNormal);
	vec3 L = normalize(inLightVec);
	vec3 V = normalize(inViewVec);
	vec3 R = reflect(L, N);
	vec3 diffuse = max(dot(N, L), 0.15) * inColour;
	vec3 specular = pow(max(dot(R, V), 0.0), 16.0) * vec3(0.75);
	outFragColour = vec4(diffuse * colour.rgb + specular, 1.0);
}
