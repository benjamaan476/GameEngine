#pragma once

#include "RendererCore.h"

struct Pipeline
{
	vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};

	vk::GraphicsPipelineCreateInfo createInfo{};
	vk::Pipeline pipeline{};
};