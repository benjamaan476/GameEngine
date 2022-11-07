#pragma once
#include "EngineCore.h"
#include "RendererState.h"

#include "Image.h"
#include "Buffer.h"
#include "CommandBuffer.h"

#include "Platform/Window.h"
#include <optional>
#include <filesystem>
#include <fstream>

#include "ui/gui.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

#include <imgui.h>
#include <imgui_impl_vulkan.h>


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

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 colour;
	glm::vec2 tex;

	static vk::VertexInputBindingDescription getBindingDescription()
	{
		vk::VertexInputBindingDescription bindingDescription{};
		bindingDescription.setBinding(0);
		bindingDescription.setStride(sizeof(Vertex));
		bindingDescription.setInputRate(vk::VertexInputRate::eVertex);
		return bindingDescription;
	}

	static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescription()
	{
		std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].setBinding(0);
		attributeDescriptions[0].setLocation(0);
		attributeDescriptions[0].setFormat(vk::Format::eR32G32B32Sfloat);
		attributeDescriptions[0].setOffset(offsetof(Vertex, pos));

		attributeDescriptions[1].setBinding(0);
		attributeDescriptions[1].setLocation(1);
		attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
		attributeDescriptions[1].setOffset(offsetof(Vertex, colour));

		attributeDescriptions[2].setBinding(0);
		attributeDescriptions[2].setLocation(2);
		attributeDescriptions[2].setFormat(vk::Format::eR32G32Sfloat);
		attributeDescriptions[2].setOffset(offsetof(Vertex, tex));
		return attributeDescriptions;
	}
};


class Application
{
public:
	Application(std::string_view name, uint32_t width, uint32_t height);
	void run();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData);

private:

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

	void initWindow(std::string_view name, uint32_t width, uint32_t height);
	void initGui();
	void initVulkan();
	void mainLoop();
	void destroyUi();
	void cleanup();
	void cleanupSwapchain();

	bool isDeviceSuitable(const vk::PhysicalDevice& device) const;
	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice) const;
	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& physicsalDevice) const;
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;
	void setupDebugMessenger();
	bool checkValidationLayerSupport() const;
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator);

	void copyBuffer(const vk::Buffer& srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
	QueueFamilyIndices findQueueFamiles(const vk::PhysicalDevice& device) const;
	std::vector<Image> getSwapchainImages(vk::SwapchainKHR swapchain);

	bool hasStencilComponent(vk::Format format) const;

	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapchain();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createDepthResources();
	void createTextureImage();
	void createTextureSampler();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffers();
	void createSyncObjects();
	void updateUniformBuffer(uint32_t currentImage);

	void recreateSwapChain();

	void drawFrame();
	vk::ShaderModule createShaderModule(const std::vector<char>& code);

	static std::vector<char> readShader(const std::filesystem::path& file);
private:

	Window::SharedPtr window;

	RendererState state{};
	VkDebugUtilsMessengerEXT debugMessenger;
	vk::SurfaceKHR surface;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

	vk::SwapchainKHR swapchain;
	std::vector<Image> swapchainImages;
	vk::ShaderModule vertShaderModule;
	vk::ShaderModule fragShaderModule;
	vk::RenderPass renderPass;
	vk::RenderPass imguiRenderPass;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorPool imguiPool;
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline graphicsPipeline;
	std::vector<vk::Framebuffer> swapChainFramebuffers;
	std::vector<vk::Framebuffer> imguiFramebuffers;
	
	Buffer vertexBuffer;
	Buffer indexBuffer;
	std::vector<Buffer> uboBuffers;
	std::vector<Buffer> fragmentUboBuffers;

	Image textureImage{};
	vk::Sampler textureSampler;

	DepthImage depthImage{};


	CommandBuffer commandBuffers;
	CommandBuffer imguiCommandBuffers;
	std::vector<vk::Semaphore> imageAvailableSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	std::vector<vk::Fence> inFlightFences;

	const std::vector<Vertex> vertices =
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
	
	const std::vector<uint32_t> indices =
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

	struct BoardProperties
	{
		glm::vec4 primaryColour{1.f, 1.f, 1.f, 1.f};
		glm::vec4 secondaryColour{0.f, 0.f, 0.f, 1.f};
		glm::ivec2 size{8, 8};
	} boardProperties;

	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation",
	};

	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	ImVec4 clearColor = { 0.f, 0.f, 0.f, 1.f };


#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	uint32_t currentFrame = 0;
	const int MaxFramesInFlight = 2;

};

