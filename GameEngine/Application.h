#pragma once
#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>

class Application
{
public:
	Application(std::string_view name, uint32_t width, uint32_t height);
	void run();

private:
	void initWindow(std::string_view name, uint32_t width, uint32_t height);
	void initVulkan();
	void mainLoop();
	void cleanup();

	
	bool isDeviceSuitable(const vk::PhysicalDevice& device) const;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	QueueFamilyIndices findQueueFamiles(const vk::PhysicalDevice& device) const;

	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();

	bool checkValidationLayerSupport() const;
private:
	std::string name;
	uint32_t width;
	uint32_t height;
	GLFWwindow* window = nullptr;

	vk::Instance instance;
	vk::PhysicalDevice physicalDevice = nullptr;
	vk::Device device{};
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	vk::SurfaceKHR surface;


	const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation",
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

};

