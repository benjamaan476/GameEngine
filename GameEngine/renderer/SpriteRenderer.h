#pragma once

#include "../EngineCore.h"

#include "Sprite.h"

namespace egkr
{
	class SpriteRenderer
	{
		static inline vk::DescriptorPool descriptorPool{};
		static inline vk::DescriptorSetLayout descriptorSetLayout{};
		static inline vk::Pipeline spritePipeline{};
		static inline Buffer vertexBuffer{};

		static inline vk::ShaderModule fragShaderModule{};
		static inline vk::ShaderModule vertShaderModule{};
	public:
		SpriteRenderer() noexcept {}
		static void create(vk::GraphicsPipelineCreateInfo basePipeline);

		static Sprite createSprite(glm::vec2 size, const Texture2D& texture);

		static void renderSprite(egkr::Sprite sprite, vk::CommandBuffer commandBuffer, uint32_t currentFrame);


		static void destroy();

	};

}