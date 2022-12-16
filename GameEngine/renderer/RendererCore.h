#pragma once

#include "RendererState.h"

#include "Buffer.h"
#include "CommandBuffer.h"
#include "RendererState.h"
#include "Texture.h"
#include "Vertex.h"

#include "../ApplicationCore.h"

#include <fstream>
namespace egkr
{
	const static inline constexpr uint32_t MaxFramesInFlight = 2u;
	constexpr uint32_t DescriptorCount = 1000;

#define DESCRIPTOR_POOL(name, type)			\
	vk::DescriptorPoolSize name{};			\
	name									\
		.setType(type) 						\
		.setDescriptorCount(DescriptorCount)

	static std::vector<char> readShader(const std::filesystem::path& filePath)
	{
		std::filesystem::path shaderPath;

		auto shaderDirectories = getShaderDirectories();

		for (const auto& path : shaderDirectories)
		{
			auto pathToShader = path / filePath;
			if (std::filesystem::exists(pathToShader))
			{
				pathToShader = std::filesystem::canonical(pathToShader);
				shaderPath = pathToShader;
				break;
			}
		}
		if (!std::filesystem::exists(shaderPath))
		{
			return {};
		}
		std::ifstream file(shaderPath.c_str(), std::ios::ate | std::ios::binary);

		ENGINE_ASSERT(file.is_open(), "Failed to open file");

		auto fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

}