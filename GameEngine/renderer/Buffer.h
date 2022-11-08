#pragma once

#include "../EngineCore.h"
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
	Buffer();
	Buffer(RendererState state, BufferProperties properties);

	void destroy();

	vk::DeviceMemory memory{};
	vk::Buffer buffer{};

private:
	RendererState state;
};
