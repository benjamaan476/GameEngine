#pragma once
#include "EngineCore.h"

#include "Platform/Window.h"
#include <optional>
#include <filesystem>
#include <fstream>

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

		std::vector<std::filesystem::path> developmentDirectories =
		{
			// First we search in source folders.
			projectDir,
			projectDir / "..",
			projectDir / ".." / "Tools" / "FalcorTest",
			// Then we search in deployment folder (necessary to pickup NVAPI and other third-party shaders).
			getExecutableDirectory() / "shaders",
		};

		std::vector<std::filesystem::path> deploymentDirectories =
		{
			getExecutableDirectory() / "Shaders",
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
	void mainLoop() const;
	void cleanup();

	
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

	QueueFamilyIndices findQueueFamiles(const vk::PhysicalDevice& device) const;

	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createGrphicsPipeline();
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
	vk::PipelineLayout pipelineLayout;

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


};

