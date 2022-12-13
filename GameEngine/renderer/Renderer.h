#pragma once

#include "RendererCore.h"
#include "SpriteRenderer.h"

#include "../BoardProperties.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

class SpriteRenderer;

class Renderer
{
public:
	static void create();
	static void cleanup();
	static void drawFrame(BoardProperties& boardProperties);

	static Renderer& get() { return *_instance; }
	static vk::Format getFormat() { return _swapchainFormat; }
	static const std::vector<Texture2D>& getSwapchainImages() { return _swapchainImages; }
	static const inline vk::Sampler& getTextureSampler() { return textureSampler; }
	static const inline vk::Sampler getSampler() { return textureSampler; }
	static const inline uint32_t getMaxFrames() { return MaxFramesInFlight; }
	static const inline vk::PipelineLayout getPipelineLayout() { return pipelineLayout; }
	static void registerImage(const Texture2D& texture);

	static void createSprite();

	//static void createImage(std::string_view imagePath);
	static vk::ShaderModule createShaderModule(const std::vector<char>& code);

	static std::vector<char> readShader(const std::filesystem::path& file);
	static void copyBuffer(const vk::Buffer& srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

private:

	static inline Renderer* _instance = nullptr;
	static inline Gui::SharedPtr _gui;

	Renderer() = default;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);

	static bool isDeviceSuitable(const vk::PhysicalDevice& device);
	static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice);
	static SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& physicsalDevice);
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
	static void setupDebugMessenger();
	static bool checkValidationLayerSupport();
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger);
	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator);

	static QueueFamilyIndices findQueueFamiles(const vk::PhysicalDevice& device);
	static std::vector<Texture2D> getSwapchainImages(vk::SwapchainKHR swapchain);

	static bool hasStencilComponent(vk::Format format);

	static void createInstance();
	static void createSurface();
	static void pickPhysicalDevice();
	static void createLogicalDevice();
	static void createSwapchain();
	static void createRenderPass();
	static void createDescriptorSetLayout();
	static void createGraphicsPipeline();
	static void createFramebuffers();
	static void createCommandPool();
	static void createDepthResources();
	static void createTextureSampler();
	static void createVertexBuffer();
	static void createIndexBuffer();
	static void createUniformBuffers();
	static void createDescriptorPool();
	static void createDescriptorSets();
	static void updateDescriptorSets();
	static void createCommandBuffers();
	static void createSyncObjects();
	static void updateUniformBuffer(BoardProperties& boardProperties, uint32_t currentImage);

	static void recreateSwapChain();
	static void cleanupSwapchain();

	static void initVulkan();

private:

	static inline VkDebugUtilsMessengerEXT _debugMessenger;
	static inline vk::SurfaceKHR surface;
	static inline vk::Format _swapchainFormat;
	static inline vk::Extent2D swapchainExtent;
	static inline vk::SwapchainKHR _swapchain;
	static inline std::vector<Texture2D> _swapchainImages;
	static inline vk::ShaderModule vertShaderModule;
	static inline vk::ShaderModule fragShaderModule;
	static inline vk::RenderPass renderPass;
	static inline vk::DescriptorSetLayout descriptorSetLayout;
	static inline vk::DescriptorPool descriptorPool;
	static inline vk::PipelineLayout pipelineLayout;
	static inline std::vector<vk::Framebuffer> swapChainFramebuffers;
	static inline Buffer vertexBuffer;
	static inline Buffer indexBuffer;
	static inline std::vector<Buffer> cameraBuffer;
	//static inline std::vector<Buffer> uboBuffers;
	static inline std::vector<Buffer> boardPropertiesBuffer;
	static inline vk::Sampler textureSampler;
	static inline DepthImage depthImage;
	static inline CommandBuffer _commandBuffers;
	static inline std::vector<vk::Semaphore> imageAvailableSemaphores;
	static inline std::vector<vk::Semaphore> renderFinishedSemaphores;
	static inline std::vector<vk::Fence> inFlightFences;

	static inline std::vector<Texture2D> _images;

	static inline struct
	{
		vk::Pipeline board;
		vk::Pipeline pieces;
	} pipelines;

	static inline struct
	{
		std::vector<vk::DescriptorSet> board;
		std::vector<vk::DescriptorSet> pieces;
	} descriptorSets;

	static const inline std::vector<Vertex> vertices
	{
		{{-0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f}, {1.f, 0.f} },
		{ {0.5f, -0.5f, 0.5f}, { 0.f, 1.f, 0.f }, {0.f, 0.f}},
		{{0.5f, 0.5f, 0.5f}, {0.f, 0.f, 1.f}, {0.f, 1.f}},
		{{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 1.f}, {1.f, 1.f}},
	};

	static const inline std::vector<uint32_t> indices =
	{
		0, 1, 2, 2, 3, 0
	};

	//struct UniformBufferObject
	//{
	//	glm::mat4 model;
	//	glm::mat4 view;
	//	glm::mat4 proj;
	//};

	static const inline std::vector<const char*> validationLayers
	{
		"VK_LAYER_KHRONOS_validation",
	};

	static const inline std::vector<const char*> deviceExtensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	static inline float4 clearColor{ 0.f, 0.f, 0.f, 1.f };

#ifdef NDEBUG
	static const inline bool enableValidationLayers = false;
#else
	static const inline bool enableValidationLayers = true;
#endif

	static inline uint32_t currentFrame;
	static const inline int MaxFramesInFlight = 2;
};

#include "Sprite.h"
class SpriteRenderer
{
	static inline std::vector<Buffer> uboBuffers{};
	static inline std::vector<Sprite> sprites{};
	static inline vk::Pipeline spritePipeline{};
	static inline std::vector<vk::DescriptorSet> spriteDescriptorSets{};
	static inline Buffer vertexBuffer{};
	static inline Sprite chessSprite{};
public:
	SpriteRenderer() {}
	static void create(vk::GraphicsPipelineCreateInfo basePipeline, std::vector<vk::DescriptorSet> descriptorSets)
	{
		auto spriteVertShaderCode = Renderer::readShader("spriteVert.spv");
		auto spriteFragShaderCode = Renderer::readShader("spriteFrag.spv");

		auto spriteVertShaderModule = Renderer::createShaderModule(spriteVertShaderCode);
		auto spriteFragShaderModule = Renderer::createShaderModule(spriteFragShaderCode);

		vk::PipelineShaderStageCreateInfo spriteVertShaderStageInfo{};
		spriteVertShaderStageInfo
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(spriteVertShaderModule)
			.setPName("main");

		vk::PipelineShaderStageCreateInfo spriteFragShaderStageInfo{};
		spriteFragShaderStageInfo
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(spriteFragShaderModule)
			.setPName("main");

		auto spriteShaderStages = { spriteVertShaderStageInfo, spriteFragShaderStageInfo };

		vk::PipelineColorBlendAttachmentState colourBlenderAttachment2{};
		colourBlenderAttachment2
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
			.setBlendEnable(true)
			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setAlphaBlendOp(vk::BlendOp::eAdd);


		vk::PipelineColorBlendStateCreateInfo colourBlending2{};
		colourBlending2
			.setLogicOpEnable(false)
			.setLogicOp(vk::LogicOp::eCopy)
			.setAttachments(colourBlenderAttachment2)
			.setBlendConstants({ 1.f, 1.f, 1.f, 1.f });

		vk::GraphicsPipelineCreateInfo spritePipelineInfo{};
		spritePipelineInfo
			.setStages(spriteShaderStages)
			.setPVertexInputState(basePipeline.pVertexInputState)
			.setPInputAssemblyState(basePipeline.pInputAssemblyState)
			.setPViewportState(basePipeline.pViewportState)
			.setPRasterizationState(basePipeline.pRasterizationState)
			.setPMultisampleState(basePipeline.pMultisampleState)
			.setPColorBlendState(&colourBlending2)
			.setPDynamicState(basePipeline.pDynamicState)
			.setPDepthStencilState(basePipeline.pDepthStencilState)
			.setLayout(basePipeline.layout)
			.setRenderPass(basePipeline.renderPass)
			.setBasePipelineIndex(-1);

		auto createInfos = { spritePipelineInfo };

		auto pipelin = state.device.createGraphicsPipelines(VK_NULL_HANDLE, createInfos).value;
		spritePipeline = pipelin[0];

		const auto maxFrames = Renderer::getMaxFrames();

		uboBuffers.resize(maxFrames);

		BufferProperties uboProperties =
		{
			.size = sizeof(ubo),
			.usage = vk::BufferUsageFlagBits::eUniformBuffer,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
		};

		for (auto i = 0u; i < maxFrames; i++)
		{
			uboBuffers[i] = Buffer(uboProperties);
		}

		vk::DeviceSize bufferSize = sizeof(Sprite::vertices[0]) * Sprite::vertices.size();

		BufferProperties properties =
		{
			.size = bufferSize,
			.usage = vk::BufferUsageFlagBits::eTransferSrc,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
		};

		auto stagingBuffer = Buffer(properties);
		auto data = state.device.mapMemory(stagingBuffer.memory, 0, bufferSize);

		std::memcpy(data, Sprite::vertices.data(), bufferSize);

		state.device.unmapMemory(stagingBuffer.memory);

		BufferProperties vertexBufferProperties =
		{
			.size = bufferSize,
			.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
		};

		vertexBuffer = Buffer(vertexBufferProperties);

		Renderer::copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, bufferSize);

		state.device.destroyBuffer(stagingBuffer.buffer);
		state.device.freeMemory(stagingBuffer.memory);

		spriteDescriptorSets = descriptorSets;

	}
	static void createSprite(glm::vec2 size, Texture2D sprite)
	{
		chessSprite = Sprite{ .size = size, .sprite = sprite };

		std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

		writeDescriptorSets.resize(0);

		for (auto i = 0u; i < Renderer::getMaxFrames(); i++)
		{

			vk::WriteDescriptorSet piecesDescriptorSet{};
			piecesDescriptorSet
				.setDstSet(spriteDescriptorSets[i])
				.setDstBinding(1)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setImageInfo(chessSprite.sprite._imageInfo);

			writeDescriptorSets.push_back(piecesDescriptorSet);

			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo
				.setBuffer(uboBuffers[i].buffer)
				.setOffset(0)
				.setRange(sizeof(ubo));

			vk::WriteDescriptorSet descriptorWrite{};
			descriptorWrite
				.setDstSet(spriteDescriptorSets[i])
				.setDstBinding(2)
				.setDstArrayElement(0)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setBufferInfo(bufferInfo);

			writeDescriptorSets.push_back(descriptorWrite);
		}

		state.device.updateDescriptorSets(writeDescriptorSets, nullptr);

	}

	static void renderSprite(vk::CommandBuffer commandBuffer, uint32_t currentFrame)
	{
		auto ortho = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);

		auto model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.5 * chessSprite.size.x, 0.5 * chessSprite.size.y, 0.f));
		model = glm::scale(model, glm::vec3(chessSprite.size, 1.f));

		auto uboData = ubo
		{
			.model = model,
			.projection = ortho
		};

		auto data = state.device.mapMemory(uboBuffers[currentFrame].memory, 0, sizeof(ubo));
		std::memcpy(data, &uboData, sizeof(ubo));
		state.device.unmapMemory(uboBuffers[currentFrame].memory);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, spritePipeline);
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Renderer::getPipelineLayout(), 0, spriteDescriptorSets[currentFrame], nullptr);

		vk::DeviceSize offsets = { 0 };
		commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
		//commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);

		commandBuffer.drawIndexed(6, 1, 0, 0, 0);
	}

	void destroy()
	{
		state.device.destroyPipeline(spritePipeline);
		vertexBuffer.destroy();
		for (auto& buffer : uboBuffers)
		{
			buffer.destroy();
		}
	}

};