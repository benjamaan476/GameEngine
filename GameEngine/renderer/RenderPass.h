#include "RendererCore.h"

#include <memory>
#include <vector>

namespace egkr
{
	struct Attachment
	{
		vk::Format format{};
		vk::SampleCountFlagBits samples{};
		vk::AttachmentLoadOp loadOp{};
		vk::AttachmentStoreOp storeOp{};
		vk::AttachmentLoadOp stencilLoadOp{};
		vk::AttachmentStoreOp stencilStoreOp{};
		vk::ImageLayout initialLayout{};
		vk::ImageLayout finalLayout{};
	};

	struct RenderPassDescription
	{
		std::string renderPassName{};
	};

	class RenderPass : public std::enable_shared_from_this<RenderPass>
	{
	public:

		using SharedPtr = std::shared_ptr<RenderPass>;
		static SharedPtr create(const RenderPassDescription& description) noexcept;
		RenderPass(const RenderPassDescription& description) noexcept;
		~RenderPass();

		const auto& get() const { return renderPass; }

		void set_colour_attachment(const Attachment& attachment);
		void set_depth_attachment(const Attachment& attachment);

		void bake();
	private:

		vk::RenderPass renderPass{};
		std::vector<vk::AttachmentDescription> colourAttachments{};
		std::vector<vk::AttachmentReference> colourAttachmentReferences{};

		vk::AttachmentDescription depthAttachment{};
		vk::AttachmentReference depthAttachmentReference{};
	};
}