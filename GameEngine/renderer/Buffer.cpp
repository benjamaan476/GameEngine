#include "Buffer.h"

Buffer::Buffer() {}

Buffer::Buffer(BufferProperties properties)
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

	_descriptor
		.setBuffer(buffer)
		.setOffset(0)
		.setRange(0);
}

void Buffer::destroy()
{
	state.device.freeMemory(memory);
	state.device.destroyBuffer(buffer);
}