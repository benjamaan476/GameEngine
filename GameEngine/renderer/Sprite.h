#pragma once

#include "../EngineCore.h"
#include "RendererCore.h"
struct Sprite
{
	static inline std::vector<Vertex> vertices
	{
		{{0.f, 1.f, 0.f}, {1.f, 1.f, 1.f}, {0.f, 1.f} },
		{{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}, {0.f, 0.f} },
		{{1.f, 0.f, 0.f}, {1.f, 1.f, 1.f}, {1.f, 0.f} },
		{{1.f, 1.f, 0.f}, {1.f, 1.f, 1.f}, {1.f, 1.f} }
	};

	glm::vec2 size;
	Texture2D sprite;
};

struct ubo
{
	glm::mat4 model;
	glm::mat4 projection;
};

