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