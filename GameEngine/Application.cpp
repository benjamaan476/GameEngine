#include "Application.h"

#include <ranges>
#include <execution>
#include <algorithm>

#include "EngineCore.h"
#include <set>


Application::Application(std::string_view name, uint32_t width, uint32_t height)

	: name(name), width(width), height(height)
{
	Log::Init();
	LOG_WARN("Initialized");
}

void Application::run()
{
	initWindow(name, width, height);
	initVulkan();

	mainLoop();
	cleanup();
}

void Application::initWindow(std::string_view name, uint32_t width, uint32_t height)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);
}

void Application::initVulkan()
{
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
}

void Application::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}

void Application::cleanup()
{
	device.destroy();
	instance.destroySurfaceKHR(surface);
	instance.destroy();

	glfwDestroyWindow(window);
	glfwTerminate();
}

bool Application::isDeviceSuitable(const vk::PhysicalDevice& device) const
{

	auto indices = findQueueFamiles(device);

	auto properties = device.getProperties();
	auto features = device.getFeatures();

	return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && features.geometryShader && indices.isComplete();
}

Application::QueueFamilyIndices Application::findQueueFamiles(const vk::PhysicalDevice& device) const
{
	QueueFamilyIndices indices;
	auto familyProperties = device.getQueueFamilyProperties();
	int i = 0;
	for (const auto& queueFamily : familyProperties)
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


void Application::createInstance()
{
	vk::ApplicationInfo info{};
	info.setPApplicationName("Hello Triangle");
	info.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
	info.setPEngineName("NoEngine");
	info.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
	info.setApiVersion(VK_API_VERSION_1_0);

	vk::InstanceCreateInfo createInfo{};
	createInfo.setPApplicationInfo(&info);

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.setEnabledExtensionCount(glfwExtensionCount);
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.setEnabledLayerCount(0);

	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("Invalid validation layer requested");
	}

	if (enableValidationLayers)
	{
		createInfo.setEnabledLayerCount((uint32_t)validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}


	instance = vk::createInstance(createInfo);
}

void Application::createSurface()
{
	glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface));
}

void Application::pickPhysicalDevice()
{
	auto devices = instance.enumeratePhysicalDevices();
	ENGINE_ASSERT(!devices.empty(), "No supported devices found");

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == vk::PhysicalDevice{})
	{
		ENGINE_ASSERT(false, "Failed to select a suitable device");
	}
}

void Application::createLogicalDevice()
{
	auto indices = findQueueFamiles(physicalDevice);

	std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
	std::set<uint32_t> uniqueQueueFamiles = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	auto queuePriority = 1.f;


	for (auto queueFamily : uniqueQueueFamiles)
	{
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo{};
		deviceQueueCreateInfo.setQueueFamilyIndex(queueFamily);
		deviceQueueCreateInfo.setQueueCount(1);

		deviceQueueCreateInfo.setQueuePriorities(queuePriority);
		deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
	}

	auto physicalDeviceFeatures = physicalDevice.getFeatures();

	vk::DeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.setPQueueCreateInfos(deviceQueueCreateInfos.data());
	deviceCreateInfo.setQueueCreateInfoCount(deviceQueueCreateInfos.size());
	deviceCreateInfo.setPEnabledFeatures(&physicalDeviceFeatures);

	deviceCreateInfo.setEnabledExtensionCount(0);

	if (enableValidationLayers)
	{
		deviceCreateInfo.setEnabledLayerCount(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		deviceCreateInfo.setEnabledLayerCount(0);
	}

	device = physicalDevice.createDevice(deviceCreateInfo);

	if (device == vk::Device{})
	{
		ENGINE_ASSERT(false, "Failed to create logical device");
	}

	graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
	presentQueue = device.getQueue(indices.presentFamily.value(), 0);

}

bool Application::checkValidationLayerSupport() const
{
	auto layers = vk::enumerateInstanceLayerProperties();

	auto validate = [&layers](const char* layer)
	{
		auto layerFound = false;
		for (const auto& layerProperty : layers)
		{
			if (strcmp(layer, layerProperty.layerName.data()) == 0)
			{
				layerFound = true;
				break;
			}
		}

		return layerFound;
	};

	return std::ranges::all_of(validationLayers, validate);
}
