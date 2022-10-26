#include "Image.h"
#include "Image.h"

uint32_t Image::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
	auto memoryProperties = state.physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	ENGINE_ASSERT(false, "Failed to find suitable memory type");
	return -1;
}

void Image::createImage()
{
	vk::ImageCreateInfo imageInfo{};
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.extent.setWidth(width);
	imageInfo.extent.setHeight(height);
	imageInfo.extent.setDepth(1);
	imageInfo.setMipLevels(1);
	imageInfo.setArrayLayers(1);
	imageInfo.setFormat(properties.format);
	imageInfo.setTiling(properties.tiling);
	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
	imageInfo.setUsage(properties.usage);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
	imageInfo.setSamples(vk::SampleCountFlagBits::e1);

	image = state.device.createImage(imageInfo);
	ENGINE_ASSERT(image != vk::Image{}, "Failed to create image");
}
void Image::createMemory()
{
	auto memoryRequirements = state.device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.setAllocationSize(memoryRequirements.size);
	allocInfo.setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, properties.memoryProperties));

	memory = state.device.allocateMemory(allocInfo);
	ENGINE_ASSERT(memory != vk::DeviceMemory{}, "Failed to allocate image memory");

	state.device.bindImageMemory(image, memory, 0);

}
void Image::createImageView()
{
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(image);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(properties.format);
	viewInfo.subresourceRange.setAspectMask(properties.aspect);
	viewInfo.subresourceRange.setBaseMipLevel(0);
	viewInfo.subresourceRange.setLevelCount(1);
	viewInfo.subresourceRange.setBaseArrayLayer(0);
	viewInfo.subresourceRange.setLayerCount(1);
	view = state.device.createImageView(viewInfo);

	ENGINE_ASSERT(view != vk::ImageView{}, "Failed to create image view");
}

void Image::transitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	auto commandBuffer = beginSingleTimeCommand();

	vk::ImageMemoryBarrier barrier{};
	barrier.setOldLayout(oldLayout);
	barrier.setNewLayout(newLayout);
	barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
	barrier.setImage(image);
	barrier.subresourceRange.setBaseMipLevel(0);
	barrier.subresourceRange.setLevelCount(1);
	barrier.subresourceRange.setBaseArrayLayer(0);
	barrier.subresourceRange.setLayerCount(1);

	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);

		if (hasStencilComponent(properties.format))
		{
			barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	else
	{
		barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
	}
	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		//barrier.setSrcAccessMask(0);
		barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
		barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		//barrier.setSrcAccessMask();
		barrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else
	{
		ENGINE_ASSERT(false, "Cannot support this image transition");
	}

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags{}, nullptr, nullptr, barrier);

	endSingleTimeCommand(commandBuffer);

}