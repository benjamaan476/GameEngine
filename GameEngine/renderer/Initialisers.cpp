#include "Initialisers.h"


namespace egkr::initialisers
{

	namespace descriptors
	{
		auto descriptor_set_layout_binding(vk::DescriptorType type, uint32_t bindingPoint, vk::ShaderStageFlags stages, uint32_t count) noexcept -> vk::DescriptorSetLayoutBinding
		{
			return vk::DescriptorSetLayoutBinding()
				.setDescriptorType(type)
				.setBinding(bindingPoint)
				.setStageFlags(stages)
				.setDescriptorCount(count);
		}

		auto descriptor_set_layout_create(const std::vector < vk::DescriptorSetLayoutBinding>& bindings) noexcept -> vk::DescriptorSetLayoutCreateInfo
		{
			return vk::DescriptorSetLayoutCreateInfo()
				.setBindings(bindings);
		}

		auto descriptor_pool_create(const std::vector<vk::DescriptorPoolSize>& poolSizes, uint32_t maxSets) noexcept -> vk::DescriptorPoolCreateInfo
		{
			vk::DescriptorPoolCreateInfo info{};
			return info
				.setPoolSizes(poolSizes)
				.setMaxSets(maxSets);
		}

		auto descriptor_allocate_info(vk::DescriptorPool descriptorPool, uint32_t count, const std::vector<vk::DescriptorSetLayout>& layouts) noexcept -> vk::DescriptorSetAllocateInfo
		{
			return vk::DescriptorSetAllocateInfo()
				.setDescriptorPool(descriptorPool)
				.setDescriptorSetCount(count)
				.setSetLayouts(layouts);
		}

		auto descriptor_write_set(vk::DescriptorSet dstSet, uint32_t bindingPoint, vk::DescriptorType type, const std::vector<vk::DescriptorBufferInfo>& bufferInfo) noexcept -> vk::WriteDescriptorSet
		{
			return vk::WriteDescriptorSet()
				.setDstSet(dstSet)
				.setDstBinding(bindingPoint)
				.setDescriptorType(type)
				.setBufferInfo(bufferInfo);
		}
	}

	namespace pipeline
	{
		auto viewportCreate(float x, float y, vk::Extent2D swapchainExtent, float minDepth, float maxDepth, vk::Offset2D offset) -> vk::PipelineViewportStateCreateInfo
		{
			vk::Viewport viewport{};
			viewport
				.setX(x)
				.setY(y)
				.setWidth((float)swapchainExtent.width)
				.setHeight((float)swapchainExtent.height)
				.setMinDepth(minDepth)
				.setMaxDepth(maxDepth);

			vk::Rect2D scissor{};
			scissor
				.setOffset(offset)
				.setExtent(swapchainExtent);

			vk::PipelineViewportStateCreateInfo viewportCreate{};
			return viewportCreate
				.setViewports(viewport)
				.setScissors(scissor);
		}

		auto vertexInputCreate() noexcept -> vk::PipelineVertexInputStateCreateInfo
		{

			auto bindingDescription = Vertex::getBindingDescription();
			auto attributeDescription = Vertex::getAttributeDescription();


			vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};

			return vertexInputInfo
				.setVertexBindingDescriptions(bindingDescription)
				//.setPVertexBindingDescriptions(&bindingDescription);
				.setVertexAttributeDescriptions(attributeDescription);
			//.setVertexAttributeDescriptionCount((uint32_t)attributeDescription.size())
			//.setPVertexAttributeDescriptions(attributeDescription.data());
		}

		auto colourBlendAttachementState(vk::ColorComponentFlags colourMask,
												   bool blendEnabled,
												   vk::BlendFactor srcColourBlend,
												   vk::BlendFactor dstColourBlend,
												   vk::BlendOp colourBlendOp,
												   vk::BlendFactor srcAlphaBlend,
												   vk::BlendFactor dstAlphaBlend,
												   vk::BlendOp alphaBlendOp) -> vk::PipelineColorBlendAttachmentState
		{
			vk::PipelineColorBlendAttachmentState colourBlendState{};

			return colourBlendState.setColorWriteMask(colourMask)
				.setBlendEnable(blendEnabled)
				.setSrcColorBlendFactor(srcColourBlend)
				.setDstColorBlendFactor(dstColourBlend)
				.setColorBlendOp(colourBlendOp)
				.setSrcAlphaBlendFactor(srcAlphaBlend)
				.setDstAlphaBlendFactor(dstAlphaBlend)
				.setAlphaBlendOp(alphaBlendOp);
		}

		auto colourBlendStateCreate(
			std::span<const vk::PipelineColorBlendAttachmentState> attachements,
			bool logicOpEnabled,
			vk::LogicOp logicOp,
			const std::array<float, 4>& blendConstants) -> vk::PipelineColorBlendStateCreateInfo
		{
			vk::PipelineColorBlendStateCreateInfo colourBlendState{};

			return colourBlendState.setAttachmentCount((uint32_t)attachements.size())
				.setPAttachments(attachements.data())
				.setLogicOpEnable(logicOpEnabled)
				.setLogicOp(logicOp)
				.setBlendConstants(blendConstants);
		}
	}
}