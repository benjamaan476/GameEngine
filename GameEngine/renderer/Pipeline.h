#pragma once

#include "RendererCore.h"

#include <memory>
namespace egkr
{
	struct PipelineDescription
	{
		std::string pipelineName;
		vk::Extent2D swapchainExtent;
		vk::PipelineLayout pipelineLayout;
		vk::RenderPass renderPass;

	};

	class Pipeline : public std::enable_shared_from_this<Pipeline>
	{
	public:
		using SharedPtr = std::shared_ptr<Pipeline>;
		static SharedPtr create(const PipelineDescription& description) noexcept;
		const auto& get_pipeline() const { return _pipeline; }

		Pipeline(const PipelineDescription& description) noexcept;
		~Pipeline();
	private:
		vk::Pipeline _pipeline{};
		vk::ShaderModule vertShaderModule{};
		vk::ShaderModule fragShaderModule{};


		vk::ShaderModule create_shader_module(const std::vector<char>& code);
	};
}