#pragma once
#include <vulkan/vulkan.hpp>


#include "Platforn/Window.h"
#include <optional>

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

	QueueFamilyIndices findQueueFamiles(const vk::PhysicalDevice& device) const;

	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();

	bool checkValidationLayerSupport() const;
private:

	Window::SharedPtr window;

	vk::Instance instance;
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

	VkDebugUtilsMessengerEXT debugMessenger;
	vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
	vk::Device device;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	vk::SurfaceKHR surface;
};

