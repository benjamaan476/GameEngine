#pragma once

#include <vulkan/vulkan.hpp>

struct RendererState
{
	vk::Instance instance{};
	vk::PhysicalDevice physicalDevice{};
	vk::Device device{};
	vk::CommandPool commandPool{};
	vk::Queue graphicsQueue{};

	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
	{
		for (const auto& format : candidates)
		{
			auto properties = physicalDevice.getFormatProperties(format);
			if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
			{
				return format;
			}

			ENGINE_ASSERT(false, "Failed to find supported format");
		}
	}

	vk::Format findDepthFormat()
	{
		return findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}


};
