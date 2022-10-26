#pragma once

#include "EngineCore.h"

#include <vulkan/vulkan.hpp>

class Image
{
	//Handle
	vk::Device device{};
	vk::PhysicalDevice physicalDevice{};

	uint32_t width{};
	uint32_t height{};

	vk::Format format{};

public:
	vk::Image image{};
	vk::DeviceMemory memory{};
	vk::ImageView view{};

public:
	Image() {}
	Image(vk::PhysicalDevice physicalDevice, vk::Device device, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::ImageAspectFlags imageAspect)
		: physicalDevice{ physicalDevice }, device{ device }, width {width}, height{ height }, format{format}
	{
		createImage(width, height, format, tiling, usage, properties);
		createImageView(format, imageAspect);
	}
	~Image()
	{

	}

	void destroy()
	{
		device.destroyImageView(view);
		device.freeMemory(memory);
		device.destroyImage(image);
	}

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
	void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
	void createImageView(vk::Format format, vk::ImageAspectFlags imageAspect);
};