#pragma once

#include "RendererState.h"
#include "Image.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "Vertex.h"

#include "../BoardProperties.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

class Renderer
{
public:
	static void create();
	static void cleanup();
	static void drawFrame(BoardProperties& boardProperties);

	static Renderer& get() { return *_instance; }
	static vk::Format getFormat() { return _swapchainFormat; }
	static const std::vector<Image>& getSwapchainImages() { return _swapchainImages; }
	static const inline Image& getTextureImage() { return textureImage; }
	static const inline vk::Sampler& getTextureSampler() { return textureSampler; }

	static void createImage(std::string_view imagePath);

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
	static std::vector<Image> getSwapchainImages(vk::SwapchainKHR swapchain);

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
	static void createTextureImage();
	static void createTextureSampler();
	static void createVertexBuffer();
	static void createIndexBuffer();
	static void createUniformBuffers();
	static void createDescriptorPool();
	static void createDescriptorSets();
	static void createCommandBuffers();
	static void createSyncObjects();
	static void updateUniformBuffer(BoardProperties& boardProperties, uint32_t currentImage);

	static void recreateSwapChain();
	static void cleanupSwapchain();

	static void initVulkan();

	static void copyBuffer(const vk::Buffer& srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);


	static vk::ShaderModule createShaderModule(const std::vector<char>& code);

	static std::vector<char> readShader(const std::filesystem::path& file);

	const static inline std::filesystem::path& getExecutableDirectory()
	{
		static std::filesystem::path directory;
		if (directory.empty())
		{
			directory = std::filesystem::current_path();
		}

		return directory;
	}

	static inline std::vector<std::filesystem::path> getInitialShaderDirectories()
	{
		std::filesystem::path projectDir(_PROJECT_DIR_);
		projectDir = projectDir.make_preferred();
		std::vector<std::filesystem::path> developmentDirectories =
		{
			// First we search in source folders.
			projectDir,
			projectDir / "shaders",
			// Then we search in deployment folder (necessary to pickup NVAPI and other third-party shaders).
			getExecutableDirectory() / "shaders",
		};

		std::vector<std::filesystem::path> deploymentDirectories =
		{
			getExecutableDirectory() / "shaders",
		};

#ifdef NDEBUG
		return deploymentDirectories;
#else
		return developmentDirectories;
#endif
	}

	static inline std::vector<std::filesystem::path> getInitialTextureDirectories()
	{
		std::filesystem::path projectDir(_PROJECT_DIR_);
		projectDir = projectDir.make_preferred();
		std::vector<std::filesystem::path> developmentDirectories =
		{
			// First we search in source folders.
			projectDir,
			projectDir / "textures",
			// Then we search in deployment folder (necessary to pickup NVAPI and other third-party shaders).
			getExecutableDirectory() / "textures",
		};

		std::vector<std::filesystem::path> deploymentDirectories =
		{
			getExecutableDirectory() / "textures",
		};

#ifdef NDEBUG
		return deploymentDirectories;
#else
		return developmentDirectories;
#endif
	}

	const static inline std::vector<std::filesystem::path> gShaderDirectories = getInitialShaderDirectories();
	const static inline std::vector<std::filesystem::path> gTextureDirectories = getInitialTextureDirectories();


private:
	static inline VkDebugUtilsMessengerEXT _debugMessenger;
	static inline vk::SurfaceKHR surface;
	static inline vk::Format _swapchainFormat;
	static inline vk::Extent2D swapchainExtent;
	static inline vk::SwapchainKHR _swapchain;
	static inline std::vector<Image> _swapchainImages;
	static inline vk::ShaderModule vertShaderModule;
	static inline vk::ShaderModule fragShaderModule;
	static inline vk::RenderPass renderPass;
	static inline vk::DescriptorSetLayout descriptorSetLayout;
	static inline vk::DescriptorPool descriptorPool;
	static inline std::vector<vk::DescriptorSet> descriptorSets;
	static inline vk::PipelineLayout pipelineLayout;
	static inline vk::Pipeline graphicsPipeline;
	static inline std::vector<vk::Framebuffer> swapChainFramebuffers;
	static inline Buffer vertexBuffer;
	static inline Buffer indexBuffer;
	static inline std::vector<Buffer> uboBuffers;
	static inline std::vector<Buffer> fragmentUboBuffers;
	static inline Image textureImage;
	static inline vk::Sampler textureSampler;
	static inline DepthImage depthImage;
	static inline CommandBuffer _commandBuffers;
	static inline std::vector<vk::Semaphore> imageAvailableSemaphores;
	static inline std::vector<vk::Semaphore> renderFinishedSemaphores;
	static inline std::vector<vk::Fence> inFlightFences;

	static const inline std::vector<Vertex> vertices
	{
		{{-0.5f, -0.5f, 0.f}, {1.f, 0.f, 0.f}, {1.f, 0.f} },
		{ {0.5f, -0.5f, 0.f}, { 0.f, 1.f, 0.f }, {0.f, 0.f}},
		{{0.5f, 0.5f, 0.f}, {0.f, 0.f, 1.f}, {0.f, 1.f}},
		{{-0.5f, 0.5f, 0.f}, {1.f, 1.f, 1.f}, {1.f, 1.f}},

		{{-0.5f, -0.5f, -.5f}, {1.f, 0.f, 0.f}, {1.f, 0.f} },
		{ {0.5f, -0.5f, -.5f}, { 0.f, 1.f, 0.f }, {0.f, 0.f}},
		{{0.5f, 0.5f, -.5f}, {0.f, 0.f, 1.f}, {0.f, 1.f}},
		{{-0.5f, 0.5f, -.5f}, {1.f, 1.f, 1.f}, {1.f, 1.f}}
	};

	static const inline std::vector<uint32_t> indices =
	{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
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
	static const inline int MaxFramesInFlight = 2;
};