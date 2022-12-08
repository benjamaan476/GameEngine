#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColour;
layout(location = 2) in vec2 inTex;

layout(location = 0) out vec3 fragColour;
layout(location = 1) out vec2 fragTex;

layout(binding = 2) uniform UniformBufferObject
{
	mat4 proj;
} camera;

void main()
{
	gl_Position = camera.proj * vec4(inPosition, 1.0);
	fragColour = inColour;
	fragTex = inTex;
}