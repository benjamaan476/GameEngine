#include "SpriteRenderer.h"

#include "Renderer.h"
#include "RendererCore.h"
#include "RendererState.h"

namespace egkr
{
	void SpriteRenderer::create(vk::GraphicsPipelineCreateInfo basePipeline)
	{
		PROFILE_FUNCTION()
		DESCRIPTOR_POOL(sampler, vk::DescriptorType::eSampler);
		DESCRIPTOR_POOL(combinedSample, vk::DescriptorType::eCombinedImageSampler);
		DESCRIPTOR_POOL(sampled, vk::DescriptorType::eSampledImage);
		DESCRIPTOR_POOL(storageImage, vk::DescriptorType::eStorageImage);
		DESCRIPTOR_POOL(uniformTexel, vk::DescriptorType::eUniformTexelBuffer);
		DESCRIPTOR_POOL(storageTexel, vk::DescriptorType::eStorageTexelBuffer);
		DESCRIPTOR_POOL(uniform, vk::DescriptorType::eUniformBuffer);
		DESCRIPTOR_POOL(storage, vk::DescriptorType::eStorageBuffer);
		DESCRIPTOR_POOL(uniformDynamic, vk::DescriptorType::eUniformBufferDynamic);
		DESCRIPTOR_POOL(storageDynamic, vk::DescriptorType::eStorageBufferDynamic);
		DESCRIPTOR_POOL(input, vk::DescriptorType::eInputAttachment);

		auto descriptorPools = { uniform, combinedSample };

		vk::DescriptorPoolCreateInfo info{};
		info.setPoolSizes(descriptorPools);
		info.setMaxSets(MaxFramesInFlight * 2);
		descriptorPool = state.device.createDescriptorPool(info);
		ENGINE_ASSERT(descriptorPool != vk::DescriptorPool{}, "Failed to create desriptor pool");


		auto spriteVertShaderCode = readShader("spriteVert.spv");
		auto spriteFragShaderCode = readShader("spriteFrag.spv");

		vertShaderModule = egakeru::createShaderModule(spriteVertShaderCode);
		fragShaderModule = egakeru::createShaderModule(spriteFragShaderCode);

		vk::PipelineShaderStageCreateInfo spriteVertShaderStageInfo{};
		spriteVertShaderStageInfo
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(vertShaderModule)
			.setPName("main");

		vk::PipelineShaderStageCreateInfo spriteFragShaderStageInfo{};
		spriteFragShaderStageInfo
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(fragShaderModule)
			.setPName("main");

		auto spriteShaderStages = { spriteVertShaderStageInfo, spriteFragShaderStageInfo };

		vk::PipelineColorBlendAttachmentState colourBlenderAttachment2{};
		colourBlenderAttachment2
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
			.setBlendEnable(true)
			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setAlphaBlendOp(vk::BlendOp::eAdd);


		vk::PipelineColorBlendStateCreateInfo colourBlending2{};
		colourBlending2
			.setLogicOpEnable(false)
			.setLogicOp(vk::LogicOp::eCopy)
			.setAttachments(colourBlenderAttachment2)
			.setBlendConstants({ 1.f, 1.f, 1.f, 1.f });

		vk::GraphicsPipelineCreateInfo spritePipelineInfo{};
		spritePipelineInfo
			.setStages(spriteShaderStages)
			.setPVertexInputState(basePipeline.pVertexInputState)
			.setPInputAssemblyState(basePipeline.pInputAssemblyState)
			.setPViewportState(basePipeline.pViewportState)
			.setPRasterizationState(basePipeline.pRasterizationState)
			.setPMultisampleState(basePipeline.pMultisampleState)
			.setPColorBlendState(&colourBlending2)
			.setPDynamicState(basePipeline.pDynamicState)
			.setPDepthStencilState(basePipeline.pDepthStencilState)
			.setLayout(basePipeline.layout)
			.setRenderPass(basePipeline.renderPass)
			.setBasePipelineIndex(-1);

		auto createInfos = { spritePipelineInfo };

		auto pipelin = state.device.createGraphicsPipelines(VK_NULL_HANDLE, createInfos).value;
		spritePipeline = pipelin[0];

		vk::DeviceSize bufferSize = sizeof(egkr::Sprite::vertices[0]) * egkr::Sprite::vertices.size();

		BufferProperties properties =
		{
			.size = bufferSize,
			.usage = vk::BufferUsageFlagBits::eTransferSrc,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
		};

		auto stagingBuffer = Buffer(properties);
		stagingBuffer.map(Sprite::vertices.data());

		BufferProperties vertexBufferProperties =
		{
			.size = bufferSize,
			.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
		};

		vertexBuffer = Buffer(vertexBufferProperties);

		vertexBuffer.copy(stagingBuffer);

		stagingBuffer.destroy();
	}

	Sprite SpriteRenderer::createSprite(glm::vec2 size, const Texture2D& texture)
	{
		PROFILE_FUNCTION()
		auto sprite = egkr::Sprite{ .size = size, .texture = texture };

		BufferProperties uboProperties =
		{
			.size = sizeof(egkr::SpriteUbo),
			.usage = vk::BufferUsageFlagBits::eUniformBuffer,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
		};

		for (size_t i = 0; i < sprite.uboBuffer.size(); i++)
		{

			sprite.uboBuffer[i] = Buffer(uboProperties);
		}

		vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding
			.setBinding(1)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
			.setStageFlags(vk::ShaderStageFlagBits::eFragment);

		vk::DescriptorSetLayoutBinding fragUboBinding{};
		fragUboBinding
			.setBinding(0)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setStageFlags(vk::ShaderStageFlagBits::eFragment);

		vk::DescriptorSetLayoutBinding cameraUboBinding{};
		cameraUboBinding
			.setBinding(2)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setStageFlags(vk::ShaderStageFlagBits::eVertex);

		auto bindings = { samplerLayoutBinding, fragUboBinding, cameraUboBinding };
		vk::DescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.setBindings(bindings);

		descriptorSetLayout = state.device.createDescriptorSetLayout(layoutInfo/*, state.allocator*/);
		ENGINE_ASSERT(descriptorSetLayout != vk::DescriptorSetLayout{}, "Failed to create descriptor set");

		//for (size_t i = 0; i < sprite.uboBuffer.size(); i++)
		{
			std::vector<vk::DescriptorSetLayout> layouts(MaxFramesInFlight, descriptorSetLayout);

			vk::DescriptorSetAllocateInfo allocInfo{};
			allocInfo
				.setDescriptorPool(descriptorPool)
				.setDescriptorSetCount(MaxFramesInFlight)
				.setSetLayouts(layouts);

			sprite.descriptor = state.device.allocateDescriptorSets(allocInfo);
		}
		std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

		writeDescriptorSets.resize(0);

		for (auto i = 0u; i < MaxFramesInFlight; i++)
		{

			vk::WriteDescriptorSet piecesDescriptorSet{};
			piecesDescriptorSet
				.setDstSet(sprite.descriptor[i])
				.setDstBinding(1)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setImageInfo(sprite.texture._imageInfo);

			writeDescriptorSets.push_back(piecesDescriptorSet);

			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo
				.setBuffer(sprite.uboBuffer[i].buffer)
				.setOffset(0)
				.setRange(sizeof(egkr::SpriteUbo));

			vk::WriteDescriptorSet descriptorWrite{};
			descriptorWrite
				.setDstSet(sprite.descriptor[i])
				.setDstBinding(2)
				.setDstArrayElement(0)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setBufferInfo(bufferInfo);

			writeDescriptorSets.push_back(descriptorWrite);
		}

		state.device.updateDescriptorSets(writeDescriptorSets, nullptr);
		return sprite;
	}

	void SpriteRenderer::renderSprite(egkr::Sprite sprite, vk::CommandBuffer commandBuffer, uint32_t currentFrame)
	{
		PROFILE_FUNCTION()
		auto ortho = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);

		auto model = glm::mat4(1.f);
		model = glm::translate(model, sprite.position);
		model = glm::translate(model, glm::vec3(0.5 * sprite.size.x, 0.5 * sprite.size.y, 0.f));
		model = glm::rotate(model, glm::radians(sprite.rotation), glm::vec3(0.f, 0.f, 1.f));
		model = glm::translate(model, glm::vec3(-0.5f * sprite.size.x, -0.5f * sprite.size.y, 0.f));
		model = glm::scale(model, glm::vec3(sprite.size, 1.f));
		//model = glm::translate(model, glm::vec3(-0.5f * sprite.size.x, -0.5f * sprite.size.y, 0.0f));
		auto uboData = egkr::SpriteUbo
		{
			.model = model,
			.projection = ortho
		};

		sprite.uboBuffer[currentFrame].map(&uboData);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, spritePipeline);
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, egakeru::getPipelineLayout(), 0, sprite.descriptor[currentFrame], nullptr);

		vk::DeviceSize offsets = { 0 };
		commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
		commandBuffer.drawIndexed(6, 1, 0, 0, 0);
	}

	void SpriteRenderer::destroy()
	{
		PROFILE_FUNCTION()
		state.device.destroyShaderModule(vertShaderModule);
		state.device.destroyShaderModule(fragShaderModule);


		state.device.destroyPipeline(spritePipeline);
		state.device.destroyDescriptorSetLayout(descriptorSetLayout);
		state.device.destroyDescriptorPool(descriptorPool);
		vertexBuffer.destroy();
	}
}