#include "Initialisers.h"

auto egkr::initialisers::pipeline::viewportCreate(float x, float y, vk::Extent2D swapchainExtent, float minDepth, float maxDepth, vk::Offset2D offset) -> vk::PipelineViewportStateCreateInfo
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

auto egkr::initialisers::pipeline::vertexInputCreate() noexcept -> vk::PipelineVertexInputStateCreateInfo
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
