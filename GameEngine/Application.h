#pragma once
#include "EngineCore.h"

#include "Platform/Window.h"
#include <optional>
#include <filesystem>
#include <fstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	glm::vec2 pos;
	glm::vec3 colour;

	static vk::VertexInputBindingDescription getBindingDescription()
	{
		vk::VertexInputBindingDescription bindingDescription{};
		bindingDescription.setBinding(0);
		bindingDescription.setStride(sizeof(Vertex));
		bindingDescription.setInputRate(vk::VertexInputRate::eVertex);
		return bindingDescription;
	}

	static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescription()
	{
	std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

	attributeDescriptions[0].setBinding(0);
	attributeDescriptions[0].setLocation(0);
	attributeDescriptions[0].setFormat(vk::Format::eR32G32Sfloat);
	attributeDescriptions[0].setOffset(offsetof(Vertex, pos));

	attributeDescriptions[1].setBinding(0);
	attributeDescriptions[1].setLocation(1);
	attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
	attributeDescriptions[1].setOffset(offsetof(Vertex, colour));
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

	const static inline std::vector<std::filesystem::path> gShaderDirectories = getInitialShaderDirectories();

	void initWindow(std::string_view name, uint32_t width, uint32_t height);
	void initVulkan();
	void mainLoop();
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
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
	void copyBuffer(const vk::Buffer& srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
	QueueFamilyIndices findQueueFamiles(const vk::PhysicalDevice& device) const;

	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapchain();
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffers();
	void createSyncObjects();
	void updateUniformBuffer(uint32_t currentImage);

	void recreateSwapChain();

	void recordCommandBuffer(const vk::CommandBuffer& commandBuffers, uint32_t imageIndex) const;

	void drawFrame();
	vk::ShaderModule createShaderModule(const std::vector<char>& code);

	static std::vector<char> readShader(const std::filesystem::path& file);
private:

	Window::SharedPtr window;

	vk::Instance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	vk::PhysicalDevice physicalDevice = nullptr;
	vk::Device device{};
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	vk::SurfaceKHR surface;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchainImages;
	std::vector<vk::ImageView> swapchainImageViews;
	vk::ShaderModule vertShaderModule;
	vk::ShaderModule fragShaderModule;
	vk::RenderPass renderPass;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline graphicsPipeline;
	std::vector<vk::Framebuffer> swapChainFramebuffers;
	vk::CommandPool commandPool;
	
	vk::Buffer vertexBuffer;
	vk::DeviceMemory vertexBufferMemory;
	vk::Buffer indexBuffer;
	vk::DeviceMemory indexBufferMemory;
	std::vector<vk::Buffer> uboBuffers;
	std::vector<vk::DeviceMemory> uboBuffersMemory;


	std::vector<vk::CommandBuffer> commandBuffers;

	std::vector<vk::Semaphore> imageAvailableSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	std::vector<vk::Fence> inFlightFences;

	const std::vector<Vertex> vertices =
	{
		{{-0.5f, -0.5f}, {1.f, 0.f, 0.f} },
		{ {0.5f, -0.5f }, { 0.f, 1.f, 0.f }},
		{{0.5f, 0.5f}, {0.f, 0.f, 1.f}},
		{{-0.5f, 0.5f}, {1.f, 1.f, 1.f}}
	};
	
	const std::vector<uint32_t> indices =
	{
		0, 1, 2, 2, 3, 0
	};

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation",
	};

	const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	uint32_t currentFrame = 0;
	const int MaxFramesInFlight = 2;

};

