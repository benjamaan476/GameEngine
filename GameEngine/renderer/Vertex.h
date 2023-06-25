#pragma once
namespace egkr
{
	struct Vertex
	{
		glm::vec4 pos;
		glm::vec3 normal;
		glm::vec2 tex;
		glm::vec3 colour;

		consteval static vk::VertexInputBindingDescription getBindingDescription() noexcept
		{
			vk::VertexInputBindingDescription bindingDescription{};
			bindingDescription.setBinding(0);
			bindingDescription.setStride(sizeof(Vertex));
			bindingDescription.setInputRate(vk::VertexInputRate::eVertex);
			return bindingDescription;
		}

		consteval static std::array<vk::VertexInputAttributeDescription, 4> getAttributeDescription() noexcept
		{
			std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions{};

			attributeDescriptions[0].setBinding(0);
			attributeDescriptions[0].setLocation(0);
			attributeDescriptions[0].setFormat(vk::Format::eR32G32B32A32Sfloat);
			attributeDescriptions[0].setOffset(offsetof(Vertex, pos));

			attributeDescriptions[1].setBinding(0);
			attributeDescriptions[1].setLocation(1);
			attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
			attributeDescriptions[1].setOffset(offsetof(Vertex, normal));

			attributeDescriptions[2].setBinding(0);
			attributeDescriptions[2].setLocation(2);
			attributeDescriptions[2].setFormat(vk::Format::eR32G32Sfloat);
			attributeDescriptions[2].setOffset(offsetof(Vertex, tex));

			attributeDescriptions[3].setBinding(0);
			attributeDescriptions[3].setLocation(3);
			attributeDescriptions[3].setFormat(vk::Format::eR32G32B32Sfloat);
			attributeDescriptions[3].setOffset(offsetof(Vertex, colour));
			return attributeDescriptions;
		}
	};
}