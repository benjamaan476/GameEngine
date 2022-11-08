#pragma once

#include "../EngineCore.h"
#include "RendererState.h"

struct ImageProperties
{
	vk::Format format;
	vk::ImageTiling tiling;
	vk::ImageUsageFlags usage;
	vk::MemoryPropertyFlags memoryProperties;
	vk::ImageAspectFlags aspect;
};


class Image
{
public:
	vk::Image image{};
	vk::DeviceMemory memory{};
	vk::ImageView view{};
	uint32_t width{};
	uint32_t height{};


public:
	Image() {}
	Image(vk::Image image, vk::Format format)
		: image{ image }
	{
		properties.format = format;
		properties.aspect = vk::ImageAspectFlagBits::eColor;

		createImageView();
	}
	Image(uint32_t width, uint32_t height, ImageProperties properties)
		: width{width}, height{height}, properties{properties}
	{
		createImage();
		createMemory();
		createImageView();
	}
	~Image()
	{

	}

	void transitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void copyFromBuffer(vk::Buffer buffer)
	{
		auto commandBuffer = beginSingleTimeCommand();

		vk::BufferImageCopy region{};
		region.setBufferOffset(0);
		region.setBufferRowLength(0);
		region.setBufferImageHeight(0);

		region.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
		region.imageSubresource.setMipLevel(0);
		region.imageSubresource.setBaseArrayLayer(0);
		region.imageSubresource.setLayerCount(1);

		region.setImageOffset({ 0, 0, 0 });
		region.setImageExtent({ width, height, 1 });

		commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
		endSingleTimeCommand(commandBuffer);
	}

	void destroyView()
	{
		state.device.destroyImageView(view);
	}

	void destroy()
	{
		destroyView();
		state.device.freeMemory(memory);
		state.device.destroyImage(image);
	}

protected:
	ImageProperties properties{};
	void createImage();
	void createMemory();
	void createImageView();

private:

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

	bool hasStencilComponent(vk::Format format) const
	{
		return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
	}

	vk::CommandBuffer beginSingleTimeCommand()
	{
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
		allocInfo.setCommandPool(state.commandPool);
		allocInfo.setCommandBufferCount(1);

		auto commandBuffers = state.device.allocateCommandBuffers(allocInfo);
		for (const auto& commandBuffer : commandBuffers)
		{
			ENGINE_ASSERT(commandBuffer != vk::CommandBuffer{}, "Failed to allocate command buffer");

		}
		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		commandBuffers[0].begin(beginInfo);
		return commandBuffers[0];
	}

	void endSingleTimeCommand(vk::CommandBuffer commandBuffer)
	{
		commandBuffer.end();

		vk::SubmitInfo submitInfo{};
		submitInfo.setCommandBufferCount(1);
		submitInfo.setCommandBuffers(commandBuffer);

		state.graphicsQueue.submit(submitInfo, vk::Fence{});
		state.graphicsQueue.waitIdle();

		state.device.freeCommandBuffers(state.commandPool, commandBuffer);
	}

};

class DepthImage : public Image
{
public:
	DepthImage() {}
	DepthImage(RendererState rendererState, uint32_t w, uint32_t h)
	{
		state = rendererState;
		width = w;
		height = h;
		properties =
		{
			.format = state.findDepthFormat(),
			.tiling = vk::ImageTiling::eOptimal,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
			.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
			.aspect = vk::ImageAspectFlagBits::eDepth
		};

		createImage();
		createMemory();
		createImageView();
	}
};