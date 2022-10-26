#include "Image.h"
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

	image = device.createImage(imageInfo);
	ENGINE_ASSERT(image != vk::Image{}, "Failed to create image");
}
void Image::createMemory()
{
	auto memoryRequirements = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocInfo{};
	allocInfo.setAllocationSize(memoryRequirements.size);
	allocInfo.setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, properties.memoryProperties));

	memory = device.allocateMemory(allocInfo);
	ENGINE_ASSERT(memory != vk::DeviceMemory{}, "Failed to allocate image memory");

	device.bindImageMemory(image, memory, 0);

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
	view = device.createImageView(viewInfo);

	ENGINE_ASSERT(view != vk::ImageView{}, "Failed to create image view");
}