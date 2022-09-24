#pragma once
#include <vulkan/vulkan.hpp>


#include "Platforn/Window.h"

#include <optional>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional < uint32_t> presentFamily;
	bool isComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
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
	void initWindow(std::string_view name, uint32_t width, uint32_t height);
	void initVulkan();
	void mainLoop() const;
	void cleanup();

	void createInstance();
	void setupDebugMessenger();
	bool isDeviceSuitable(vk::PhysicalDevice device) const;
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();

	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) const
	{
		QueueFamilyIndices indices;
		auto queueFamilyProperties = device.getQueueFamilyProperties();


		int i = 0;
		for (const auto& queueFamily : queueFamilyProperties)
		{
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.graphicsFamily = i;
			}

			auto presentSupport = device.getSurfaceSupportKHR(i, surface);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			if (indices.isComplete())
			{
				break;
			}
			i++;
		}

		return indices;
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator);


	bool checkValidationLayerSupport() const;
private:

	Window::SharedPtr window;

	vk::Instance instance;
	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation",
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

