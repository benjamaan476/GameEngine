#include "EngineCore.h"
#include "Application.h"
#include "Log/Log.h"

#include <ranges>
#include <execution>
#include <algorithm>
#include <set>

Application::Application(std::string_view name, uint32_t width, uint32_t height)
{
	Log::Init();
	LOG_WARN("Initialized");
	initWindow(name, width, height);
}

void Application::run()
{
	initVulkan();
	mainLoop();
	cleanup();
}

VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
	switch (messageSeverity)
	{
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		LOG_INFO("{0}", callbackData->pMessage);
		break;
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		LOG_WARN("{0}", callbackData->pMessage);
		break;
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		LOG_ERROR("{0}", callbackData->pMessage);
		break;
	//case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	//	LOG_TRACE("{0}", callbackData->pMessage);
	//default:
	//	LOG_ERROR("{0}", "Debug layer");
	}

	return true;
}


void Application::initWindow(std::string_view name, uint32_t width, uint32_t height)
{
	WindowProperties windowProperties{ name.data(), width, height};
	window = Window::create(windowProperties);
}

void Application::initVulkan()
{
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	setupDebugMessenger();
}

void Application::mainLoop() const
{
	while (!window->isRunning())
	{
		window->OnUpdate();
	}
}

void Application::cleanup()
{
	for (auto imageView : swapchainImageViews) 
	{
		device.destroyImageView(imageView);
	}
	device.destroySwapchainKHR(swapchain);
	device.destroy();
	instance.destroySurfaceKHR(surface);
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	instance.destroy();
}

bool Application::isDeviceSuitable(const vk::PhysicalDevice& device) const
{

	auto indices = findQueueFamiles(device);

	auto properties = device.getProperties();
	auto features = device.getFeatures();

	auto swapChainSupport = querySwapChainSupport(device);
	auto swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

	return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && features.geometryShader && indices.isComplete() && swapChainAdequate;
}

bool Application::checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice) const
{
	auto extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : extensionProperties)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportDetails Application::querySwapChainSupport(const vk::PhysicalDevice& physicalDevice) const
{
	SwapChainSupportDetails details;

	details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
	details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

	return details;
}

vk::SurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}
	LOG_WARN("No ideal format found, picking first format");
	return availableFormats[0];
}

vk::PresentModeKHR Application::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
		{
			return availablePresentMode;
		}
	}

	LOG_WARN("No ideal present mode found, picking first mode");
	return availablePresentModes[0];
}

vk::Extent2D Application::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		vk::Extent2D actualExtent =
		{
			(uint32_t)width, 
			(uint32_t)height
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

QueueFamilyIndices Application::findQueueFamiles(const vk::PhysicalDevice& device) const
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

	auto extensions = window->GetRequiredExtensions();

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	createInfo.setEnabledExtensionCount(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.setEnabledLayerCount(0);

	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		ENGINE_ASSERT(false, "Invalid validation layer requested");
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

	auto supported = checkDeviceExtensionSupport(physicalDevice);
	ENGINE_ASSERT(supported, "Device does not support required extensions");
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

	deviceCreateInfo.setEnabledExtensionCount(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

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

void Application::setupDebugMessenger()
{
	if (!enableValidationLayers)
	{
	}
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.messageSeverity = 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

	createInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

	createInfo.pUserData = nullptr;
	createInfo.pfnUserCallback = debugCallback;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	}
	{
		ENGINE_ASSERT("{0}", "Failed to set up debug messenger");
	
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

void Application::createSwapChain()
{
	auto swapChainSupportDetails = querySwapChainSupport(physicalDevice);

	auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
	auto presentMode = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
	auto extent = chooseSwapExtent(swapChainSupportDetails.capabilities);

	auto imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

	if (swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.maxImageCount)
	{
		imageCount = swapChainSupportDetails.capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo{};
	createInfo.setSurface(surface);
	createInfo.setMinImageCount(imageCount);
	createInfo.setImageFormat(surfaceFormat.format);
	createInfo.setImageColorSpace(surfaceFormat.colorSpace);
	createInfo.setImageExtent(extent);
	createInfo.setImageArrayLayers(1);
	createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

	auto indices = findQueueFamiles(physicalDevice);

	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
		createInfo.setQueueFamilyIndexCount(2);
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
		createInfo.setQueueFamilyIndexCount(0);
		createInfo.pQueueFamilyIndices = nullptr;

	}

	createInfo.setPreTransform(swapChainSupportDetails.capabilities.currentTransform);
	createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	createInfo.setPresentMode(presentMode);
	createInfo.setClipped(true);
	//createInfo.setOldSwapchain();

	swapchain = device.createSwapchainKHR(createInfo);

	if (swapchain == vk::SwapchainKHR{})
	{
		ENGINE_ASSERT(false, "Failed to create swap chain");
	}

	swapchainImages = device.getSwapchainImagesKHR(swapchain);

	ENGINE_ASSERT(!swapchainImages.empty(), "Failed to retrieve swapchain images");
	swapchainFormat = surfaceFormat.format;
	swapchainExtent = extent;
}

void Application::createImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());

	for (int i = 0; i < swapchainImages.size(); i++)
	{
		vk::ImageViewCreateInfo createInfo{};

		createInfo.setImage(swapchainImages[i]);
		createInfo.setViewType(vk::ImageViewType::e2D);
		createInfo.setFormat(swapchainFormat);
		createInfo.setComponents(vk::ComponentMapping());

		vk::ImageSubresourceRange subresourceRange{};
		subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
		subresourceRange.setBaseMipLevel(0);
		subresourceRange.setLevelCount(1);
		subresourceRange.setBaseArrayLayer(0);
		subresourceRange.setLayerCount(1);

		createInfo.setSubresourceRange(subresourceRange);

		swapchainImageViews[i] = device.createImageView(createInfo);
	}
}
