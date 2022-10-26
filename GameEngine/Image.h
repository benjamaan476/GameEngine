#pragma once

#include "EngineCore.h"

#include <vulkan/vulkan.hpp>

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
	//Handle
	vk::Device device{};
	vk::PhysicalDevice physicalDevice{};

	uint32_t width{};
	uint32_t height{};
	ImageProperties properties;
public:
	vk::Image image{};
	vk::DeviceMemory memory{};
	vk::ImageView view{};


public:
	Image() {}
	Image(vk::PhysicalDevice physicalDevice, vk::Device device, uint32_t width, uint32_t height, ImageProperties properties)
		: physicalDevice{ physicalDevice }, device{ device }, width{ width }, height{ height }, properties{ properties }
	{
		createImage();
		createMemory();
		createImageView();
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
private:

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
	void createImage();
	void createMemory();
	void createImageView();
};