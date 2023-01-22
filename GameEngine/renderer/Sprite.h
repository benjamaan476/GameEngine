#pragma once

#include "../EngineCore.h"
#include "RendererCore.h"
#include "../ApplicationCore.h"
namespace egkr
{
	struct SpriteUbo
	{
		glm::mat4 model{};
		glm::mat4 projection{};
	};

	struct Sprite
	{
		static inline std::vector<Vertex> vertices
		{
			{{0.f, 1.f, 0.f}, {1.f, 1.f, 1.f}, {0.f, 0.f} },
			{{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}, {0.f, 1.f} },
			{{1.f, 0.f, 0.f}, {1.f, 1.f, 1.f}, {1.f, 1.f} },
			{{1.f, 1.f, 0.f}, {1.f, 1.f, 1.f}, {1.f, 0.f} }
		};

		float2 size{};
		float3 position{};
		float rotation{};
		Texture2D texture;
		std::vector<vk::DescriptorSet> descriptor{};
		std::array<Buffer, 2> uboBuffer{};
		SpriteUbo ubo{};

		void destory()
		{
			for (auto& buffer : uboBuffer)
			{
				buffer.destroy();
			}
		}

		~Sprite()
		{

		}
	};



}