#include "Pipeline.h"
#include "../ApplicationCore.h"
#include "Initialisers.h"

namespace egkr
{

	std::vector<char> readShader(const std::filesystem::path& filePath)
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

	vk::ShaderModule Pipeline::create_shader_module(const std::vector<char>& code)
	{
		vk::ShaderModuleCreateInfo createInfo{};

		createInfo.setCodeSize(code.size());
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		auto module = state.device.createShaderModule(createInfo);
		ENGINE_ASSERT(module != vk::ShaderModule{}, "Failed to create shader module");

		return module;
	}

	Pipeline::SharedPtr Pipeline::create(const PipelineDescription& description) noexcept
	{
		return std::make_shared<Pipeline>(description);
	}

	Pipeline::Pipeline(const PipelineDescription& description) noexcept
	{
		auto boardVertShaderCode = readShader("vert.spv");
		ENGINE_ASSERT(!boardVertShaderCode.empty(), "Failed to create shader");

		auto boardFragSharedCode = readShader("frag.spv");
		ENGINE_ASSERT(!boardFragSharedCode.empty(), "Failed to create shader");

		vertShaderModule = create_shader_module(boardVertShaderCode);
		fragShaderModule = create_shader_module(boardFragSharedCode);

		auto vertShaderStageInfo = initialisers::pipeline::shaderCreate(vk::ShaderStageFlagBits::eVertex, "main", vertShaderModule);
		auto fragShaderStageInfo = initialisers::pipeline::shaderCreate(vk::ShaderStageFlagBits::eFragment, "main", fragShaderModule);

		auto shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

		auto dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.setDynamicStates(dynamicStates);

		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescription = Vertex::getAttributeDescription();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo
			.setVertexBindingDescriptions(bindingDescription)
			.setVertexAttributeDescriptions(attributeDescription);

		auto inputAssembly = initialisers::pipeline::inputAssemblyCreate(vk::PrimitiveTopology::eTriangleList, false);
		auto viewportState = initialisers::pipeline::viewportCreate(0.F, 0.F, description.swapchainExtent, 0.F, 1.F, { 0, 0 });
		auto rasterizer = initialisers::pipeline::rasterizationCreate(false, false, vk::PolygonMode::eFill, 1.F, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, false, 0.F, 0.F, 0.F);
		auto multisample = initialisers::pipeline::multisampleCreate(false, vk::SampleCountFlagBits::e1, 1.F, false, false);
		auto colourMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		auto blendOne = vk::BlendFactor::eOne;
		auto add = vk::BlendOp::eAdd;

		auto colourBlendAttachmentState = initialisers::pipeline::colourBlendAttachementState(colourMask, false, blendOne, blendOne, add, blendOne, vk::BlendFactor::eZero, add);
		auto attachments = { colourBlendAttachmentState };
		auto colourBlendCreate = initialisers::pipeline::colourBlendStateCreate(attachments, false, vk::LogicOp::eCopy, { 0.F, 0.F, 0.F, 0.F });
		auto depthStencilInfo = initialisers::pipeline::depthStencilCreate(true, true, vk::CompareOp::eLess, false, false);

		vk::GraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo
			.setStages(shaderStages)
			.setPVertexInputState(&vertexInputInfo)
			.setPInputAssemblyState(&inputAssembly)
			.setPViewportState(&viewportState)
			.setPRasterizationState(&rasterizer)
			.setPMultisampleState(&multisample)
			.setPColorBlendState(&colourBlendCreate)
			.setPDynamicState(&dynamicStateCreateInfo)
			.setPDepthStencilState(&depthStencilInfo)
			.setLayout(description.pipelineLayout)
			.setRenderPass(description.renderPass)
			.setSubpass(0)
			.setBasePipelineIndex(-1);

		auto createInfo = { pipelineInfo };

		auto pipelin = state.device.createGraphicsPipelines(VK_NULL_HANDLE, createInfo).value;
		_pipeline = pipelin[0];
		ENGINE_ASSERT(_pipeline != vk::Pipeline{}, "Failed to create graphics pipeline");
	}

	Pipeline::~Pipeline()
	{
		state.device.destroyPipeline(_pipeline);
	}
}
