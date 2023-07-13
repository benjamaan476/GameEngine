#pragma once

#include "RendererCore.h"


namespace egkr::initialisers
{

	namespace descriptors
	{

		[[nodiscard]] auto descriptor_set_layout_binding(vk::DescriptorType type, uint32_t bindingPoint, vk::ShaderStageFlags stages, uint32_t count) noexcept -> vk::DescriptorSetLayoutBinding;
		[[nodiscard]] auto descriptor_set_layout_create(const std::vector<vk::DescriptorSetLayoutBinding>& bindings) noexcept -> vk::DescriptorSetLayoutCreateInfo;
		[[nodiscard]] auto descriptor_pool_create(const std::vector<vk::DescriptorPoolSize>& poolSizes, uint32_t maxSets) noexcept -> vk::DescriptorPoolCreateInfo;
		[[nodiscard]] auto descriptor_allocate_info(vk::DescriptorPool descriptorPool, uint32_t count, const std::vector<vk::DescriptorSetLayout>& layouts) noexcept -> vk::DescriptorSetAllocateInfo;
		[[nodiscard]] auto descriptor_write_set(vk::DescriptorSet dstSet, uint32_t bindingPoint, vk::DescriptorType type, const std::vector<vk::DescriptorBufferInfo>& bufferInfo) noexcept -> vk::WriteDescriptorSet;
	}

	namespace pipeline
	{
		auto viewportCreate(float x, float y, vk::Extent2D swapchainExtent, float minDepth, float maxDepth, vk::Offset2D offset) -> vk::PipelineViewportStateCreateInfo;


		constexpr auto shaderCreate(vk::ShaderStageFlagBits shaderStage, std::string_view entry, vk::ShaderModule module) noexcept
		{
			vk::PipelineShaderStageCreateInfo shaderStageInfo{};
			return shaderStageInfo
				.setStage(shaderStage)
				.setPName(entry.data())
				.setModule(module);
		}

		auto vertexInputCreate() noexcept -> vk::PipelineVertexInputStateCreateInfo;


		consteval auto inputAssemblyCreate(vk::PrimitiveTopology topology, bool restartEnabled) noexcept
		{
			vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
			
			return inputAssembly
				.setTopology(topology)
				.setPrimitiveRestartEnable(restartEnabled);
		}

		consteval auto rasterizationCreate(bool depthClampEnabled, bool discardEnabled, vk::PolygonMode mode, float lineWidth, vk::CullModeFlagBits cullMode, vk::FrontFace frontFace, bool biasEnabled, float biadConstantFactor, float biasClamp, float biasSlopeFactor)
		{
			vk::PipelineRasterizationStateCreateInfo rasterizationCreate{};

			return rasterizationCreate
				.setDepthClampEnable(depthClampEnabled)
				.setRasterizerDiscardEnable(discardEnabled)
				.setPolygonMode(mode)
				.setLineWidth(lineWidth)
				.setCullMode(cullMode)
				.setFrontFace(frontFace)
				.setDepthBiasEnable(biasEnabled)
				.setDepthBiasConstantFactor(biadConstantFactor)
				.setDepthBiasClamp(biasClamp)
				.setDepthBiasSlopeFactor(biasSlopeFactor);
		}

		consteval auto multisampleCreate(bool sampleShadingEnabled, vk::SampleCountFlagBits samples, float minSampleShading, bool alphaCoverageEnabled, bool alphaToOneEnabled)
		{
			vk::PipelineMultisampleStateCreateInfo multisampleCreate{};

			return multisampleCreate
				.setSampleShadingEnable(sampleShadingEnabled)
				.setRasterizationSamples(samples)
				.setMinSampleShading(minSampleShading)
				.setPSampleMask(nullptr)
				.setAlphaToCoverageEnable(alphaCoverageEnabled)
				.setAlphaToOneEnable(alphaToOneEnabled);
		}

		auto colourBlendAttachementState(vk::ColorComponentFlags colourMask,
                  bool blendEnabled,
                  vk::BlendFactor srcColourBlend,
                  vk::BlendFactor dstColourBlend,
                  vk::BlendOp colourBlendOp,
                  vk::BlendFactor srcAlphaBlend,
                  vk::BlendFactor dstAlphaBlend,
                  vk::BlendOp alphaBlendOp) -> vk::PipelineColorBlendAttachmentState;

		consteval auto depthStencilCreate(bool enabled, bool writeEnabled, vk::CompareOp compareOp, bool boundsEnabled, bool stencilEnabled)
		{
			vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};

			return depthStencilInfo
				.setDepthTestEnable(enabled)
				.setDepthWriteEnable(writeEnabled)
				.setDepthCompareOp(compareOp)
				.setDepthBoundsTestEnable(boundsEnabled)
				.setStencilTestEnable(stencilEnabled);
		}

		auto colourBlendStateCreate(std::span<const vk::PipelineColorBlendAttachmentState> attachements,
                  bool logicOpEnabled,
                  vk::LogicOp logicOp,
                  const std::array<float, 4> &blendConstants) -> vk::PipelineColorBlendStateCreateInfo;

	}
}
