//#pragma once
//
//#include "Sprite.h"
//#include "../EngineCore.h"
//
//#include "RendererCore.h"
//#include "RendererState.h"
//
//class SpriteRenderer
//{
//	static std::vector<Buffer> uboBuffers;
//	static std::vector<Sprite> sprites;
//	static vk::Pipeline spritePipeline;
//	static std::vector<vk::DescriptorSet> spriteDescriptorSets;
//
//	static Buffer vertexBuffer;
//
//
//	static Sprite chessSprite;
//public:
//	SpriteRenderer() {}
//	static void create()
//	{
//		auto spriteVertShaderCode = Renderer::readShader("spriteVert.spv");
//		auto spriteFragShaderCode = Renderer::readShader("spriteFrag.spv");
//
//		auto spriteVertShaderModule = Renderer::createShaderModule(spriteVertShaderCode);
//		auto spriteFragShaderModule = Renderer::createShaderModule(spriteFragShaderCode);
//
//		vk::PipelineShaderStageCreateInfo spriteVertShaderStageInfo{};
//		spriteVertShaderStageInfo
//			.setStage(vk::ShaderStageFlagBits::eVertex)
//			.setModule(spriteVertShaderModule)
//			.setPName("main");
//
//		vk::PipelineShaderStageCreateInfo spriteFragShaderStageInfo{};
//		spriteFragShaderStageInfo
//			.setStage(vk::ShaderStageFlagBits::eFragment)
//			.setModule(spriteFragShaderModule)
//			.setPName("main");
//
//		auto spriteShaderStages = { spriteVertShaderStageInfo, spriteFragShaderStageInfo };
//
//		vk::PipelineColorBlendAttachmentState colourBlenderAttachment2{};
//		colourBlenderAttachment2
//			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
//			.setBlendEnable(true)
//			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
//			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
//			.setColorBlendOp(vk::BlendOp::eAdd)
//			.setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha)
//			.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
//			.setAlphaBlendOp(vk::BlendOp::eAdd);
//
//
//		vk::PipelineColorBlendStateCreateInfo colourBlending2{};
//		colourBlending2
//			.setLogicOpEnable(false)
//			.setLogicOp(vk::LogicOp::eCopy)
//			.setAttachments(colourBlenderAttachment2)
//			.setBlendConstants({ 1.f, 1.f, 1.f, 1.f });
//
//		vk::GraphicsPipelineCreateInfo spritePipelineInfo{};
//		spritePipelineInfo
//			.setStages(spriteShaderStages)
//			.setPColorBlendState(&colourBlending2)
//			.setBasePipelineIndex(0);
//
//		auto createInfos = { spritePipelineInfo };
//
//		auto pipelin = state.device.createGraphicsPipelines(VK_NULL_HANDLE, createInfos).value;
//		spritePipeline = pipelin[0];
//
//		const auto maxFrames = Renderer::getMaxFrames();
//
//		uboBuffers.resize(maxFrames);
//
//		BufferProperties uboProperties =
//		{
//			.size = sizeof(ubo),
//			.usage = vk::BufferUsageFlagBits::eUniformBuffer,
//			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
//		};
//
//		for (auto i = 0u; i < maxFrames; i++)
//		{
//			uboBuffers[i] = Buffer(uboProperties);
//		}
//
//		vk::DeviceSize bufferSize = sizeof(Sprite::vertices[0]) * Sprite::vertices.size();
//
//		BufferProperties properties =
//		{
//			.size = bufferSize,
//			.usage = vk::BufferUsageFlagBits::eTransferSrc,
//			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
//		};
//
//		auto stagingBuffer = Buffer(properties);
//		auto data = state.device.mapMemory(stagingBuffer.memory, 0, bufferSize);
//
//		std::memcpy(data, Sprite::vertices.data(), bufferSize);
//
//		state.device.unmapMemory(stagingBuffer.memory);
//
//		BufferProperties vertexBufferProperties =
//		{
//			.size = bufferSize,
//			.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
//			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
//		};
//
//		vertexBuffer = Buffer(vertexBufferProperties);
//
//		Renderer::copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, bufferSize);
//
//		state.device.destroyBuffer(stagingBuffer.buffer);
//		state.device.freeMemory(stagingBuffer.memory);
//
//	}
//	static void createSprite(glm::vec2 size, Texture2D sprite)
//	{
//		chessSprite = Sprite{ .size = size, .sprite = sprite };
//	}
//
//	static void renderSprite(vk::CommandBuffer commandBuffer, const Sprite& sprite, uint32_t currentFrame)
//	{
//		auto ortho = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);
//
//		auto model = glm::mat4(1.f);
//		model = glm::translate(model, glm::vec3(0.5 * sprite.size.x, 0.5 * sprite.size.y, 0.f));
//		model = glm::scale(model, glm::vec3(sprite.size, 1.f));
//
//		auto uboData = ubo
//		{
//			.model = model,
//			.projection = ortho
//		};
//
//		auto data = state.device.mapMemory(uboBuffers[currentFrame].memory, 0, sizeof(ubo));
//		std::memcpy(data, &uboData, sizeof(ubo));
//		state.device.unmapMemory(uboBuffers[currentFrame].memory);
//
//		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Renderer::getPipelineLayout(), 0, spriteDescriptorSets[currentFrame], nullptr);
//		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, spritePipeline);
//
//		vk::DeviceSize offsets = { 0 };
//		commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
//		//commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);
//
//		commandBuffer.drawIndexed(sprite.vertices.size(), 1, 0, 0, 0);
//
//
//	}
//
//	void destroy()
//	{
//		state.device.destroyPipeline(spritePipeline);
//		vertexBuffer.destroy();
//		for (auto& buffer : uboBuffers)
//		{
//			buffer.destroy();
//		}
//	}
//
//};