//#pragma once
//
//#include "../EngineCore.h"
//
//#include "RendererCore.h"
//#include "RendererState.h"
//
//#include "Sprite.h"
//
//namespace egkr
//{
//	class SpriteRenderer
//	{
//		//static inline std::vector<Buffer> uboBuffers{};
//		static inline vk::Pipeline spritePipeline{};
//		static inline Buffer vertexBuffer{};
//	public:
//		SpriteRenderer() {}
//		static void create(vk::GraphicsPipelineCreateInfo basePipeline, std::vector<vk::DescriptorSet> descriptorSets)
//		{
//			auto spriteVertShaderCode = readShader("spriteVert.spv");
//			auto spriteFragShaderCode = readShader("spriteFrag.spv");
//
//			auto spriteVertShaderModule = egakeru::createShaderModule(spriteVertShaderCode);
//			auto spriteFragShaderModule = egakeru::createShaderModule(spriteFragShaderCode);
//
//			vk::PipelineShaderStageCreateInfo spriteVertShaderStageInfo{};
//			spriteVertShaderStageInfo
//				.setStage(vk::ShaderStageFlagBits::eVertex)
//				.setModule(spriteVertShaderModule)
//				.setPName("main");
//
//			vk::PipelineShaderStageCreateInfo spriteFragShaderStageInfo{};
//			spriteFragShaderStageInfo
//				.setStage(vk::ShaderStageFlagBits::eFragment)
//				.setModule(spriteFragShaderModule)
//				.setPName("main");
//
//			auto spriteShaderStages = { spriteVertShaderStageInfo, spriteFragShaderStageInfo };
//
//			vk::PipelineColorBlendAttachmentState colourBlenderAttachment2{};
//			colourBlenderAttachment2
//				.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
//				.setBlendEnable(true)
//				.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
//				.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
//				.setColorBlendOp(vk::BlendOp::eAdd)
//				.setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha)
//				.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
//				.setAlphaBlendOp(vk::BlendOp::eAdd);
//
//
//			vk::PipelineColorBlendStateCreateInfo colourBlending2{};
//			colourBlending2
//				.setLogicOpEnable(false)
//				.setLogicOp(vk::LogicOp::eCopy)
//				.setAttachments(colourBlenderAttachment2)
//				.setBlendConstants({ 1.f, 1.f, 1.f, 1.f });
//
//			vk::GraphicsPipelineCreateInfo spritePipelineInfo{};
//			spritePipelineInfo
//				.setStages(spriteShaderStages)
//				.setPVertexInputState(basePipeline.pVertexInputState)
//				.setPInputAssemblyState(basePipeline.pInputAssemblyState)
//				.setPViewportState(basePipeline.pViewportState)
//				.setPRasterizationState(basePipeline.pRasterizationState)
//				.setPMultisampleState(basePipeline.pMultisampleState)
//				.setPColorBlendState(&colourBlending2)
//				.setPDynamicState(basePipeline.pDynamicState)
//				.setPDepthStencilState(basePipeline.pDepthStencilState)
//				.setLayout(basePipeline.layout)
//				.setRenderPass(basePipeline.renderPass)
//				.setBasePipelineIndex(-1);
//
//			auto createInfos = { spritePipelineInfo };
//
//			auto pipelin = state.device.createGraphicsPipelines(VK_NULL_HANDLE, createInfos).value;
//			spritePipeline = pipelin[0];
//
//			vk::DeviceSize bufferSize = sizeof(egkr::Sprite::vertices[0]) * egkr::Sprite::vertices.size();
//
//			BufferProperties properties =
//			{
//				.size = bufferSize,
//				.usage = vk::BufferUsageFlagBits::eTransferSrc,
//				.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
//			};
//
//			auto stagingBuffer = Buffer(properties);
//			auto data = state.device.mapMemory(stagingBuffer.memory, 0, bufferSize);
//			std::memcpy(data, egkr::Sprite::vertices.data(), bufferSize);
//			state.device.unmapMemory(stagingBuffer.memory);
//
//			BufferProperties vertexBufferProperties =
//			{
//				.size = bufferSize,
//				.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
//				.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
//			};
//
//			vertexBuffer = Buffer(vertexBufferProperties);
//
//			vertexBuffer.copy(stagingBuffer);
//
//			state.device.destroyBuffer(stagingBuffer.buffer);
//			state.device.freeMemory(stagingBuffer.memory);
//		}
//
//		static egkr::Sprite createSprite(glm::vec2 size, Texture2D texture)
//		{
//			auto sprite = egkr::Sprite{ .size = size, .texture = texture };
//
//			for (size_t i = 0; i < sprite.uboBuffer.size(); i++)
//			{
//				BufferProperties uboProperties =
//				{
//					.size = sizeof(egkr::SpriteUbo),
//					.usage = vk::BufferUsageFlagBits::eUniformBuffer,
//					.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
//				};
//
//				for (auto i = 0u; i < MaxFramesInFlight; i++)
//				{
//					sprite.uboBuffer[i] = Buffer(uboProperties);
//				}
//			}
//			std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
//
//			writeDescriptorSets.resize(0);
//
//			for (auto i = 0u; i < MaxFramesInFlight; i++)
//			{
//
//				vk::WriteDescriptorSet piecesDescriptorSet{};
//				piecesDescriptorSet
//					.setDstSet(sprite.descriptor[i])
//					.setDstBinding(1)
//					.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
//					.setImageInfo(sprite.texture._imageInfo);
//
//				writeDescriptorSets.push_back(piecesDescriptorSet);
//
//				vk::DescriptorBufferInfo bufferInfo{};
//				bufferInfo
//					.setBuffer(sprite.uboBuffer[i].buffer)
//					.setOffset(0)
//					.setRange(sizeof(egkr::SpriteUbo));
//
//				vk::WriteDescriptorSet descriptorWrite{};
//				descriptorWrite
//					.setDstSet(sprite.descriptor[i])
//					.setDstBinding(2)
//					.setDstArrayElement(0)
//					.setDescriptorType(vk::DescriptorType::eUniformBuffer)
//					.setBufferInfo(bufferInfo);
//
//				writeDescriptorSets.push_back(descriptorWrite);
//			}
//
//			state.device.updateDescriptorSets(writeDescriptorSets, nullptr);
//			return sprite;
//		}
//
//		static void renderSprite(egkr::Sprite sprite, vk::CommandBuffer commandBuffer, uint32_t currentFrame)
//		{
//			auto ortho = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);
//
//			auto model = glm::mat4(1.f);
//			model = glm::translate(model, glm::vec3(0.5 * sprite.size.x, 0.5 * sprite.size.y, 0.f));
//			model = glm::scale(model, glm::vec3(sprite.size, 1.f));
//
//			auto uboData = egkr::SpriteUbo
//			{
//				.model = model,
//				.projection = ortho
//			};
//
//			auto data = state.device.mapMemory(sprite.uboBuffer[currentFrame].memory, 0, sizeof(egkr::SpriteUbo));
//			std::memcpy(data, &uboData, sizeof(egkr::SpriteUbo));
//			state.device.unmapMemory(sprite.uboBuffer[currentFrame].memory);
//
//			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, spritePipeline);
//			commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, egakeru::getPipelineLayout(), 0, sprite.descriptor[currentFrame], nullptr);
//
//			vk::DeviceSize offsets = { 0 };
//			commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
//			//commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);
//
//			commandBuffer.drawIndexed(6, 1, 0, 0, 0);
//		}
//
//		void destroy()
//		{
//			state.device.destroyPipeline(spritePipeline);
//			vertexBuffer.destroy();
//		}
//
//	};
//
//}