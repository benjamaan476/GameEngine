#include "RenderPass.h"

namespace egkr
{
	RenderPass::SharedPtr RenderPass::create(const RenderPassDescription& description) noexcept
	{
		return std::make_shared<RenderPass>(description);
	}

	RenderPass::RenderPass(const RenderPassDescription&) noexcept
	{
		vk::AttachmentDescription colouAttachment{};
	}

	RenderPass::~RenderPass()
	{
		state.device.destroyRenderPass(renderPass);
	}

	void RenderPass::set_colour_attachment(const Attachment& attachment)
	{
		vk::AttachmentDescription attachmentDescription{};
		attachmentDescription
			.setFormat(attachment.format)
			.setSamples(attachment.samples)
			.setLoadOp(attachment.loadOp)
			.setStoreOp(attachment.storeOp)
			.setStencilLoadOp(attachment.stencilLoadOp)
			.setStencilStoreOp(attachment.stencilStoreOp)
			.setInitialLayout(attachment.initialLayout)
			.setFinalLayout(attachment.finalLayout);

		vk::AttachmentReference attachmentReference{};
		attachmentReference
			.setAttachment((uint32_t)colourAttachmentReferences.size())
			.setLayout(attachment.finalLayout);

		colourAttachments.emplace_back(attachmentDescription);
		colourAttachmentReferences.emplace_back(attachmentReference);
	}

	void RenderPass::set_depth_attachment(const Attachment& attachment)
	{
		vk::AttachmentDescription attachmentDescription{};
		attachmentDescription
			.setFormat(attachment.format)
			.setSamples(attachment.samples)
			.setLoadOp(attachment.loadOp)
			.setStoreOp(attachment.storeOp)
			.setStencilLoadOp(attachment.stencilLoadOp)
			.setStencilStoreOp(attachment.stencilStoreOp)
			.setInitialLayout(attachment.initialLayout)
			.setFinalLayout(attachment.finalLayout);

		vk::AttachmentReference attachmentReference{};
		attachmentReference
			.setAttachment((uint32_t)colourAttachmentReferences.size())
			.setLayout(attachment.finalLayout);

		depthAttachment = attachmentDescription;
		depthAttachmentReference = attachmentReference;
	}

	void RenderPass::bake()
	{
		vk::SubpassDescription subpass{};
		subpass
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachments(colourAttachmentReferences)
			.setPDepthStencilAttachment(&depthAttachmentReference);

		vk::SubpassDependency dependency{};
		dependency
			.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

		auto attachments = colourAttachments;
		attachments.push_back(depthAttachment);

		vk::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo
			.setAttachments(attachments)
			.setSubpasses(subpass)
			.setDependencies(dependency);

		renderPass = state.device.createRenderPass(renderPassInfo);
		ENGINE_ASSERT(renderPass != vk::RenderPass{}, "Failed to create render pass");
	}
}