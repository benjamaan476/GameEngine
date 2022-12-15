#pragma once

#include "RendererCore.h"
#include "Sprite.h"
#include "SpriteRenderer.h"

#include "../BoardProperties.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace egkr
{
	class SpriteRenderer;

	class egakeru
	{
	public:
		static void create();
		static void cleanup();
		static void drawFrame(const BoardProperties& boardProperties, const Sprite& sprite);

		static egakeru& get() { return *_instance; }
		static vk::Format getFormat() { return _swapchainFormat; }
		static const std::vector<Texture2D>& getSwapchainImages() { return _swapchainImages; }
		static const inline vk::Sampler& getTextureSampler() { return textureSampler; }
		static const inline vk::Sampler getSampler() { return textureSampler; }
		static const inline vk::PipelineLayout getPipelineLayout() { return pipelineLayout; }

		static egkr::Sprite createSprite(const Texture2D& texture);

		//static void createImage(std::string_view imagePath);
		static vk::ShaderModule createShaderModule(const std::vector<char>& code);
	private:

		static inline egakeru* _instance = nullptr;
		static inline Gui::SharedPtr _gui;

		egakeru() = default;

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
		static void createCommandBuffers();
		static void createSyncObjects();
		static void updateUniformBuffer(const BoardProperties& boardProperties, uint32_t currentImage);

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
		static inline std::vector<Buffer> boardPropertiesBuffer;
		static inline vk::Sampler textureSampler;
		static inline DepthImage depthImage;
		static inline CommandBuffer _commandBuffers;
		static inline std::vector<vk::Semaphore> imageAvailableSemaphores;
		static inline std::vector<vk::Semaphore> renderFinishedSemaphores;
		static inline std::vector<vk::Fence> inFlightFences;

		static inline struct
		{
			vk::Pipeline board;
			vk::Pipeline pieces;
		} pipelines;

		static inline struct
		{
			std::vector<vk::DescriptorSet> board;
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
	};

	class SpriteRenderer
	{
		//static inline std::vector<Buffer> uboBuffers{};
		static inline vk::DescriptorPool descriptorPool{};
		static inline vk::DescriptorSetLayout descriptorSetLayout{};
		static inline vk::Pipeline spritePipeline{};
		static inline Buffer vertexBuffer{};

		static inline vk::ShaderModule fragShaderModule{};
		static inline vk::ShaderModule vertShaderModule{};
	public:
		SpriteRenderer() {}
		static void create(vk::GraphicsPipelineCreateInfo basePipeline)
		{
			DESCRIPTOR_POOL(sampler, vk::DescriptorType::eSampler);
			DESCRIPTOR_POOL(combinedSample, vk::DescriptorType::eCombinedImageSampler);
			DESCRIPTOR_POOL(sampled, vk::DescriptorType::eSampledImage);
			DESCRIPTOR_POOL(storageImage, vk::DescriptorType::eStorageImage);
			DESCRIPTOR_POOL(uniformTexel, vk::DescriptorType::eUniformTexelBuffer);
			DESCRIPTOR_POOL(storageTexel, vk::DescriptorType::eStorageTexelBuffer);
			DESCRIPTOR_POOL(uniform, vk::DescriptorType::eUniformBuffer);
			DESCRIPTOR_POOL(storage, vk::DescriptorType::eStorageBuffer);
			DESCRIPTOR_POOL(uniformDynamic, vk::DescriptorType::eUniformBufferDynamic);
			DESCRIPTOR_POOL(storageDynamic, vk::DescriptorType::eStorageBufferDynamic);
			DESCRIPTOR_POOL(input, vk::DescriptorType::eInputAttachment);

			auto descriptorPools = { uniform, combinedSample };

			vk::DescriptorPoolCreateInfo info{};
			info.setPoolSizes(descriptorPools);
			info.setMaxSets(MaxFramesInFlight * 2);
			descriptorPool = state.device.createDescriptorPool(info);
			ENGINE_ASSERT(descriptorPool != vk::DescriptorPool{}, "Failed to create desriptor pool");


			auto spriteVertShaderCode = readShader("spriteVert.spv");
			auto spriteFragShaderCode = readShader("spriteFrag.spv");

			vertShaderModule = egakeru::createShaderModule(spriteVertShaderCode);
			fragShaderModule = egakeru::createShaderModule(spriteFragShaderCode);

			vk::PipelineShaderStageCreateInfo spriteVertShaderStageInfo{};
			spriteVertShaderStageInfo
				.setStage(vk::ShaderStageFlagBits::eVertex)
				.setModule(vertShaderModule)
				.setPName("main");

			vk::PipelineShaderStageCreateInfo spriteFragShaderStageInfo{};
			spriteFragShaderStageInfo
				.setStage(vk::ShaderStageFlagBits::eFragment)
				.setModule(fragShaderModule)
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

			vk::DeviceSize bufferSize = sizeof(egkr::Sprite::vertices[0]) * egkr::Sprite::vertices.size();

			BufferProperties properties =
			{
				.size = bufferSize,
				.usage = vk::BufferUsageFlagBits::eTransferSrc,
				.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
			};

			auto stagingBuffer = Buffer(properties);
			stagingBuffer.map(Sprite::vertices.data());

			BufferProperties vertexBufferProperties =
			{
				.size = bufferSize,
				.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
				.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
			};

			vertexBuffer = Buffer(vertexBufferProperties);

			vertexBuffer.copy(stagingBuffer);

			stagingBuffer.destroy();
		}

		static egkr::Sprite createSprite(glm::vec2 size,const Texture2D& texture)
		{
			auto sprite = egkr::Sprite{ .size = size, .texture = texture };

			BufferProperties uboProperties =
			{
				.size = sizeof(egkr::SpriteUbo),
				.usage = vk::BufferUsageFlagBits::eUniformBuffer,
				.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
			};

			for (size_t i = 0; i < sprite.uboBuffer.size(); i++)
			{

				sprite.uboBuffer[i] = Buffer(uboProperties);	
			}

			vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
			samplerLayoutBinding
				.setBinding(1)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment);

			vk::DescriptorSetLayoutBinding fragUboBinding{};
			fragUboBinding
				.setBinding(0)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment);

			vk::DescriptorSetLayoutBinding cameraUboBinding{};
			cameraUboBinding
				.setBinding(2)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setStageFlags(vk::ShaderStageFlagBits::eVertex);

			auto bindings = { samplerLayoutBinding, fragUboBinding, cameraUboBinding };
			vk::DescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.setBindings(bindings);

			descriptorSetLayout = state.device.createDescriptorSetLayout(layoutInfo/*, state.allocator*/);
			ENGINE_ASSERT(descriptorSetLayout != vk::DescriptorSetLayout{}, "Failed to create descriptor set");

			//for (size_t i = 0; i < sprite.uboBuffer.size(); i++)
			{
				std::vector<vk::DescriptorSetLayout> layouts(MaxFramesInFlight, descriptorSetLayout);

				vk::DescriptorSetAllocateInfo allocInfo{};
				allocInfo
					.setDescriptorPool(descriptorPool)
					.setDescriptorSetCount(MaxFramesInFlight)
					.setSetLayouts(layouts);

				sprite.descriptor = state.device.allocateDescriptorSets(allocInfo);
			}
			std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

			writeDescriptorSets.resize(0);

			for (auto i = 0u; i < MaxFramesInFlight; i++)
			{

				vk::WriteDescriptorSet piecesDescriptorSet{};
				piecesDescriptorSet
					.setDstSet(sprite.descriptor[i])
					.setDstBinding(1)
					.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
					.setImageInfo(sprite.texture._imageInfo);

				writeDescriptorSets.push_back(piecesDescriptorSet);

				vk::DescriptorBufferInfo bufferInfo{};
				bufferInfo
					.setBuffer(sprite.uboBuffer[i].buffer)
					.setOffset(0)
					.setRange(sizeof(egkr::SpriteUbo));

				vk::WriteDescriptorSet descriptorWrite{};
				descriptorWrite
					.setDstSet(sprite.descriptor[i])
					.setDstBinding(2)
					.setDstArrayElement(0)
					.setDescriptorType(vk::DescriptorType::eUniformBuffer)
					.setBufferInfo(bufferInfo);

				writeDescriptorSets.push_back(descriptorWrite);
			}

			state.device.updateDescriptorSets(writeDescriptorSets, nullptr);
			return sprite;
		}

		static void renderSprite(egkr::Sprite sprite, vk::CommandBuffer commandBuffer, uint32_t currentFrame)
		{
			auto ortho = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);

			auto model = glm::mat4(1.f);
			model = glm::translate(model, glm::vec3(0.5 * sprite.size.x, 0.5 * sprite.size.y, 0.f));
			model = glm::scale(model, glm::vec3(sprite.size, 1.f));
			//model = glm::translate(model, glm::vec3(-0.5f * sprite.size.x, -0.5f * sprite.size.y, 0.0f));
			auto uboData = egkr::SpriteUbo
			{
				.model = model,
				.projection = ortho
			};

			sprite.uboBuffer[currentFrame].map(&uboData);

			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, spritePipeline);
			commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, egakeru::getPipelineLayout(), 0, sprite.descriptor[currentFrame], nullptr);

			vk::DeviceSize offsets = { 0 };
			commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
			commandBuffer.drawIndexed(6, 1, 0, 0, 0);
		}

		static void destroy()
		{
			state.device.destroyShaderModule(vertShaderModule);
			state.device.destroyShaderModule(fragShaderModule);


			state.device.destroyPipeline(spritePipeline);
			state.device.destroyDescriptorSetLayout(descriptorSetLayout);
			state.device.destroyDescriptorPool(descriptorPool);
			vertexBuffer.destroy();
		}
	};
}