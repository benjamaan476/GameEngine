#include "Image.h"

uint32_t Image::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
	auto memoryProperties = physicalDevice.getMemoryProperties();

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

void Image::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
{
	vk::ImageCreateInfo imageInfo{};
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.extent.setWidth(width);
	imageInfo.extent.setHeight(height);
	imageInfo.extent.setDepth(1);
	imageInfo.setMipLevels(1);
	imageInfo.setArrayLayers(1);
	imageInfo.setFormat(format);
	imageInfo.setTiling(tiling);
	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
	imageInfo.setUsage(usage);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
	imageInfo.setSamples(vk::SampleCountFlagBits::e1);

	image = device.createImage(imageInfo);
	if (image == vk::Image{})
	{
		ENGINE_ASSERT(false, "Failed to create image");
	}
	auto memoryRequirements = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.setAllocationSize(memoryRequirements.size);
	allocInfo.setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, properties));

	memory = device.allocateMemory(allocInfo);
	if (memory == vk::DeviceMemory{})
	{
		ENGINE_ASSERT(false, "Failed to allocate image memory");
	}
	device.bindImageMemory(image, memory, 0);

}
void Image::createImageView(vk::Format format, vk::ImageAspectFlags imageAspects)
{
	vk::ImageViewCreateInfo viewInfo{};
	viewInfo.setImage(image);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.subresourceRange.setAspectMask(imageAspects);
	viewInfo.subresourceRange.setBaseMipLevel(0);
	viewInfo.subresourceRange.setLevelCount(1);
	viewInfo.subresourceRange.setBaseArrayLayer(0);
	viewInfo.subresourceRange.setLayerCount(1);
	view = device.createImageView(viewInfo);

	if (view == vk::ImageView{})
	{
		ENGINE_ASSERT(false, "FAiled to create image view");
	}
}