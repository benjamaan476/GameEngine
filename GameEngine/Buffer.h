#pragma once

#include "EngineCore.h"
#include "RendererState.h"

#include <vulkan/vulkan.hpp>

struct BufferProperties
{
	vk::DeviceSize size;
	vk::BufferUsageFlags usage;
	vk::MemoryPropertyFlags memoryProperties;
};

class Buffer
{
public:
	Buffer() {}
	Buffer(RendererState state, BufferProperties properties)
		: state{ state }
	{
		vk::BufferCreateInfo bufferInfo{};
		bufferInfo.setSize(properties.size);
		bufferInfo.setUsage(properties.usage);
		bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

		buffer = state.device.createBuffer(bufferInfo);
		ENGINE_ASSERT(buffer != vk::Buffer{}, "Failed to create vertex buffer");


		auto memoryRequirements = state.device.getBufferMemoryRequirements(buffer);

		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.setAllocationSize(memoryRequirements.size);
		allocInfo.setMemoryTypeIndex(state.findMemoryType(memoryRequirements.memoryTypeBits, properties.memoryProperties));

		memory = state.device.allocateMemory(allocInfo);
		ENGINE_ASSERT(memory != vk::DeviceMemory{}, "Failed to allocate vertex memory");

		state.device.bindBufferMemory(buffer, memory, 0);
	}

	void destroy()
	{
		state.device.freeMemory(memory);
		state.device.destroyBuffer(buffer);
	}

	vk::DeviceMemory memory{};
	vk::Buffer buffer{};

private:
	RendererState state;
};

class CommandBuffer : public Buffer
{
public:
	CommandBuffer() {}
	CommandBuffer(RendererState state, size_t bufferCount)
	{
		commandBuffers.resize(bufferCount);

		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.setCommandPool(state.commandPool);
		allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
		allocInfo.setCommandBufferCount(static_cast<uint32_t>(commandBuffers.size()));

		commandBuffers = state.device.allocateCommandBuffers(allocInfo);

		for (const auto& commandBuffer : commandBuffers)
		{
			ENGINE_ASSERT(commandBuffer != vk::CommandBuffer{}, "Failed to create command buffer");
		}

	}

	void record(uint32_t bufferIndex, uint32_t imageIndex, std::function<void(vk::CommandBuffer)> recordBuffer)
	{
		auto& commandBuffer = commandBuffers[bufferIndex];

		commandBuffer.reset();

		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.setPInheritanceInfo(nullptr);
		commandBuffer.begin(beginInfo);

		recordBuffer(commandBuffer);

		commandBuffer.end();		
	}

	vk::CommandBuffer operator[] (uint32_t index)
	{
		return commandBuffers[index];
	}

	const vk::CommandBuffer operator[] (uint32_t index) const
	{
		return commandBuffers[index];
	}

private:

	std::vector<vk::CommandBuffer> commandBuffers;
};