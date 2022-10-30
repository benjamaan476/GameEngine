#include "Application.h"
#include "Log/Log.h"

#include <ranges>
#include <execution>
#include <algorithm>
#include <set>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Application::Application(std::string_view name, uint32_t width, uint32_t height)
{
	Log::Init();
	LOG_INFO("Initialized");
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
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapchain();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createCommandPool();
	createDepthResources();
	createFramebuffers();
	createTextureImage();
	createTextureSampler();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();
}

void Application::mainLoop()
{
	while (!window->isRunning())
	{
		window->OnUpdate();
		drawFrame();
	}

	state.device.waitIdle();
}

bool Application::isDeviceSuitable(const vk::PhysicalDevice& device) const
{
	auto indices = findQueueFamiles(device);

	auto properties = device.getProperties();
	auto features = device.getFeatures();

	auto swapChainSupport = querySwapChainSupport(device);
	auto swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

	return properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu && features.geometryShader && indices.isComplete() && swapChainAdequate;
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
		if (availablePresentMode == vk::PresentModeKHR::eFifo)
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
		auto [width, height] = window->getFramebufferSize();
		vk::Extent2D actualExtent =
		{
			width, 
			height
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




bool Application::hasStencilComponent(vk::Format format) const
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}


vk::CommandBuffer Application::beginSingleTimeCommand()
{
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandPool(state.commandPool);
	allocInfo.setCommandBufferCount(1);

	auto commandBuffers = state.device.allocateCommandBuffers(allocInfo);
	for (const auto& commandBuffer : commandBuffers)
	{
		ENGINE_ASSERT(commandBuffer != vk::CommandBuffer{}, "Failed to allocate command buffer");
	
	} 
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	commandBuffers[0].begin(beginInfo);
	return commandBuffers[0];
}

void Application::endSingleTimeCommand(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo{};
	submitInfo.setCommandBufferCount(1);
	submitInfo.setCommandBuffers(commandBuffer);

	state.graphicsQueue.submit(submitInfo, vk::Fence{});
	state.graphicsQueue.waitIdle();

	state.device.freeCommandBuffers(state.commandPool, commandBuffer);
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

	createInfo.setEnabledExtensionCount((uint32_t)extensions.size());
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


	state.instance = vk::createInstance(createInfo);
	ENGINE_ASSERT(state.instance != vk::Instance{}, "Failed to create instance");
}

void Application::createSurface()
{
	surface = window->createSurface(state.instance);
}

void Application::pickPhysicalDevice()
{
	auto devices = state.instance.enumeratePhysicalDevices();
	ENGINE_ASSERT(!devices.empty(), "No supported devices found");

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			state.physicalDevice = device;
			break;
		}
	}
	ENGINE_ASSERT(state.physicalDevice != vk::PhysicalDevice{}, "Failed to select a suitable device");

	auto supported = checkDeviceExtensionSupport(state.physicalDevice);
	ENGINE_ASSERT(supported, "Device does not support required extensions");
}

void Application::createLogicalDevice()
{
	auto indices = findQueueFamiles(state.physicalDevice);

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

	auto physicalDeviceFeatures = state.physicalDevice.getFeatures();

	vk::DeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.setQueueCreateInfos(deviceQueueCreateInfos);
	deviceCreateInfo.setPEnabledFeatures(&physicalDeviceFeatures);

	deviceCreateInfo.setEnabledExtensionCount((uint32_t)deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		deviceCreateInfo.setEnabledLayerCount((uint32_t)validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		deviceCreateInfo.setEnabledLayerCount(0);
	}

	state.device = state.physicalDevice.createDevice(deviceCreateInfo);

	ENGINE_ASSERT(state.device != vk::Device{}, "Failed to create logical device");

	state.graphicsQueue = state.device.getQueue(indices.graphicsFamily.value(), 0);
	presentQueue = state.device.getQueue(indices.presentFamily.value(), 0);

}

void Application::setupDebugMessenger()
{
	if (!enableValidationLayers)
	{
		return;
	} 

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
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


	if (CreateDebugUtilsMessengerEXT(state.instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		ENGINE_ASSERT(false, "Failed to set up debug messenger");
	}
}

VkResult Application::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		return func(instance, createInfo, allocator, debugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

}

void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, allocator);
	}
}

void Application::copyBuffer(const vk::Buffer& srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	auto commandBuffer = beginSingleTimeCommand();

	vk::BufferCopy copyRegion{};
	copyRegion.setSrcOffset(0);
	copyRegion.setDstOffset(0);
	copyRegion.setSize(size);

	commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

	endSingleTimeCommand(commandBuffer);
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

void Application::createSwapchain()
{
	auto swapChainSupportDetails = querySwapChainSupport(state.physicalDevice);

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

	auto indices = findQueueFamiles(state.physicalDevice);

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

	swapchain = state.device.createSwapchainKHR(createInfo);

	ENGINE_ASSERT(swapchain != vk::SwapchainKHR{}, "Failed to create swap chain");

	swapchainFormat = surfaceFormat.format;
	swapchainImages = getSwapchainImages(swapchain);
	ENGINE_ASSERT(!swapchainImages.empty(), "Failed to retrieve swapchain images");
	swapchainExtent = extent;
}

void Application::createRenderPass()
{
	vk::AttachmentDescription colourAttachment{};
	colourAttachment.setFormat(swapchainFormat);
	colourAttachment.setSamples(vk::SampleCountFlagBits::e1);
	colourAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
	colourAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
	colourAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	colourAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	colourAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
	colourAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference colourAttachmentRef{};
	colourAttachmentRef.setAttachment(0);
	colourAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

	vk::AttachmentDescription depthAttachment{};
	depthAttachment.setFormat(state.findDepthFormat());
	depthAttachment.setSamples(vk::SampleCountFlagBits::e1);
	depthAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
	depthAttachment.setStoreOp(vk::AttachmentStoreOp::eDontCare);
	depthAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	depthAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	depthAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
	depthAttachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentReference depthAttachmentRef{};
	depthAttachmentRef.setAttachment(1);
	depthAttachmentRef.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpass{};
	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
	subpass.setColorAttachments(colourAttachmentRef);
	subpass.setPDepthStencilAttachment(&depthAttachmentRef);

	vk::SubpassDependency dependency{};
	dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
	dependency.setDstSubpass(0);
	dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests);
	dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests);
	dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

	auto attachments = { colourAttachment, depthAttachment };

	vk::RenderPassCreateInfo renderPassInfo{};
	renderPassInfo.setAttachments(attachments);
	renderPassInfo.setSubpasses(subpass);
	renderPassInfo.setDependencies(dependency);

	renderPass = state.device.createRenderPass(renderPassInfo);

	ENGINE_ASSERT(renderPass != vk::RenderPass{}, "Failed to create render pass");
}

void Application::createDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.setBinding(0);
	uboLayoutBinding.setDescriptorCount(1);
	uboLayoutBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
	uboLayoutBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex);
	//uboLayoutBinding.setPImmutableSamplers(nullptr);

	vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.setBinding(1);
	samplerLayoutBinding.setDescriptorCount(1);
	samplerLayoutBinding.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
	samplerLayoutBinding.setStageFlags(vk::ShaderStageFlagBits::eFragment);
	//samplerLayoutBinding.setImmutableSamplers(nullptr);

	auto bindings = { uboLayoutBinding, samplerLayoutBinding };
	vk::DescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.setBindings(bindings);

	descriptorSetLayout = state.device.createDescriptorSetLayout(layoutInfo);
	ENGINE_ASSERT(descriptorSetLayout != vk::DescriptorSetLayout{}, "Failed to create descriptor set");
}

void Application::createGraphicsPipeline()
{
	std::filesystem::path vertShader("vert.spv");
	auto vertShaderCode = readShader(vertShader);
	auto fragShaderCode = readShader("frag.spv");

	vertShaderModule = createShaderModule(vertShaderCode);
	fragShaderModule = createShaderModule(fragShaderCode);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
	vertShaderStageInfo.setModule(vertShaderModule);
	vertShaderStageInfo.setPName("main");

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
	fragShaderStageInfo.setModule(fragShaderModule);
	fragShaderStageInfo.setPName("main");

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	std::vector<vk::DynamicState> dynamicStates =
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{};

	dynamicStateCreateInfo.setDynamicStates(dynamicStates);

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescription = Vertex::getAttributeDescription();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.setVertexBindingDescriptions(bindingDescription);
	vertexInputInfo.setVertexAttributeDescriptions(attributeDescription);

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
	inputAssembly.setPrimitiveRestartEnable(false);

	vk::Viewport viewport{};
	viewport.setX(0.f);
	viewport.setY(0.f);
	viewport.setWidth((float)swapchainExtent.width);
	viewport.setHeight((float)swapchainExtent.height);
	viewport.setMinDepth(0.f);
	viewport.setMaxDepth(1.f);

	vk::Rect2D scissor{};
	scissor.setOffset({ 0, 0 });
	scissor.setExtent(swapchainExtent);

	vk::PipelineViewportStateCreateInfo viewportState{};
	viewportState.setViewports(viewport);
	viewportState.setScissors(scissor);

	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.setDepthClampEnable(false);
	rasterizer.setRasterizerDiscardEnable(false);
	rasterizer.setPolygonMode(vk::PolygonMode::eFill);
	rasterizer.setLineWidth(1.f);
	rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
	rasterizer.setFrontFace(vk::FrontFace::eCounterClockwise);

	rasterizer.setDepthBiasEnable(false);
	rasterizer.setDepthBiasConstantFactor(0.f);
	rasterizer.setDepthBiasClamp(0.f);
	rasterizer.setDepthBiasSlopeFactor(0.f);

	vk::PipelineMultisampleStateCreateInfo multiSample{};

	multiSample.setSampleShadingEnable(false);
	multiSample.setRasterizationSamples(vk::SampleCountFlagBits::e1);
	multiSample.setMinSampleShading(1.f);
	multiSample.setPSampleMask(nullptr);
	multiSample.setAlphaToCoverageEnable(false);
	multiSample.setAlphaToOneEnable(false);

	vk::PipelineColorBlendAttachmentState colourBlenderAttachment{};
	colourBlenderAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	colourBlenderAttachment.setBlendEnable(false);
	colourBlenderAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
	colourBlenderAttachment.setDstColorBlendFactor(vk::BlendFactor::eSrc1Alpha);
	colourBlenderAttachment.setColorBlendOp(vk::BlendOp::eAdd);
	colourBlenderAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
	colourBlenderAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
	colourBlenderAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

	vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};
	depthStencilInfo.setDepthTestEnable(true);
	depthStencilInfo.setDepthWriteEnable(true);
	depthStencilInfo.setDepthCompareOp(vk::CompareOp::eLess);
	depthStencilInfo.setDepthBoundsTestEnable(false);
	depthStencilInfo.setStencilTestEnable(false);


	vk::PipelineColorBlendStateCreateInfo colourBlending{};
	colourBlending.setLogicOpEnable(false);
	colourBlending.setLogicOp(vk::LogicOp::eCopy);
	colourBlending.setAttachments(colourBlenderAttachment);
	colourBlending.setBlendConstants({ 0.f, 0.f, 0.f, 0.f });


	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.setPushConstantRangeCount(0);
	pipelineLayoutInfo.setPPushConstantRanges(nullptr);
	pipelineLayoutInfo.setSetLayouts(descriptorSetLayout);

	pipelineLayout = state.device.createPipelineLayout(pipelineLayoutInfo);
	ENGINE_ASSERT(pipelineLayout != vk::PipelineLayout{}, "FAiled to create pipeline layout");

	vk::GraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.setStageCount(2);
	pipelineInfo.setPStages(shaderStages);
	pipelineInfo.setPVertexInputState(&vertexInputInfo);
	pipelineInfo.setPInputAssemblyState(&inputAssembly);
	pipelineInfo.setPViewportState(&viewportState);
	pipelineInfo.setPRasterizationState(&rasterizer);
	pipelineInfo.setPMultisampleState(&multiSample);
	pipelineInfo.setPDepthStencilState(nullptr);
	pipelineInfo.setPColorBlendState(&colourBlending);
	pipelineInfo.setPDynamicState(&dynamicStateCreateInfo);
	pipelineInfo.setPDepthStencilState(&depthStencilInfo);
	pipelineInfo.setLayout(pipelineLayout);
	pipelineInfo.setRenderPass(renderPass);
	pipelineInfo.setSubpass(0);
	//pipelineInfo.setBasePipelineHandle(VK_NULL_HANDLE);
	pipelineInfo.setBasePipelineIndex(-1);

	vk::PipelineCache cache{};

	auto success = vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(&pipelineInfo), nullptr, reinterpret_cast<VkPipeline*>(&graphicsPipeline));
	ENGINE_ASSERT(success == VK_SUCCESS, "Failed to create graphics pipeline");
}

void Application::createFramebuffers()
{
	swapChainFramebuffers.resize(0);
	for (const auto& image : swapchainImages)
	{
		const auto attachments =
		{
			image.view,
			depthImage.view
		};

		vk::FramebufferCreateInfo framebufferInfo{};
		framebufferInfo.setRenderPass(renderPass);
		framebufferInfo.setAttachments(attachments);
		framebufferInfo.setWidth(swapchainExtent.width);
		framebufferInfo.setHeight(swapchainExtent.height);
		framebufferInfo.setLayers(1);

		auto framebuffer = state.device.createFramebuffer(framebufferInfo);
		ENGINE_ASSERT(framebuffer != vk::Framebuffer{}, "Failed to create framebuffer");
		swapChainFramebuffers.emplace_back(framebuffer);
	}

}

void Application::createCommandPool()
{
	auto queuFamilyIndices = findQueueFamiles(state.physicalDevice);

	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	poolInfo.setQueueFamilyIndex(queuFamilyIndices.graphicsFamily.value());

	state.commandPool = state.device.createCommandPool(poolInfo);
	ENGINE_ASSERT(state.commandPool != vk::CommandPool{}, "FAiled to create command pool");
}

void Application::createDepthResources()
{
	auto format = state.findDepthFormat();
	ImageProperties properties
	{
		.format = format,
		.tiling = vk::ImageTiling::eOptimal,
		.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
		.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
		.aspect = vk::ImageAspectFlagBits::eDepth
	};
	depthImage = DepthImage(state, swapchainExtent.width, swapchainExtent.height);
	depthImage.transitionLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
}

void Application::createTextureImage()
{
	int width, height, channels;
	stbi_uc* pixels = nullptr;

	auto filePath = "texture.jpeg";

	for (const auto& path : gTextureDirectories)
	{
		auto pathToImage = path / filePath;
		if (std::filesystem::exists(pathToImage))
		{
			pixels = stbi_load(pathToImage.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			break;
		}
	}

	ENGINE_ASSERT(pixels, "Failed to load image");

	vk::DeviceSize imageSize = width * height * 4;

	BufferProperties properties =
	{
		.size = imageSize,
		.usage = vk::BufferUsageFlagBits::eTransferSrc,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible,
	};

	auto stagingBuffer = Buffer(state, properties);
	auto data = state.device.mapMemory(stagingBuffer.memory, 0, imageSize);
	std::memcpy(data, pixels, imageSize);
	state.device.unmapMemory(stagingBuffer.memory);

	stbi_image_free(pixels);

	ImageProperties imageProperties
	{
		.format = vk::Format::eB8G8R8A8Srgb,
		.tiling = vk::ImageTiling::eOptimal,
		.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
		.aspect = vk::ImageAspectFlagBits::eColor
	};

	textureImage = Image(state, width, height, imageProperties);
	textureImage.transitionLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	textureImage.copyFromBuffer(stagingBuffer.buffer);
	textureImage.transitionLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

	state.device.destroyBuffer(stagingBuffer.buffer);
	state.device.freeMemory(stagingBuffer.memory);
}

void Application::createTextureSampler()
{
	vk::SamplerCreateInfo samplerInfo{};
	samplerInfo.setMagFilter(vk::Filter::eLinear);
	samplerInfo.setMinFilter(vk::Filter::eLinear);
	samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setAnisotropyEnable(true);

	auto properties = state.physicalDevice.getProperties();
	samplerInfo.setMaxAnisotropy(properties.limits.maxSamplerAnisotropy);
	
	samplerInfo.setBorderColor(vk::BorderColor::eIntOpaqueBlack);
	samplerInfo.setUnnormalizedCoordinates(false);
	samplerInfo.setCompareEnable(false);
	samplerInfo.setCompareOp(vk::CompareOp::eAlways);
	samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
	samplerInfo.setMipLodBias(0.f);
	samplerInfo.setMinLod(0.f);
	samplerInfo.setMaxLod(0.f);

	textureSampler = state.device.createSampler(samplerInfo);
	ENGINE_ASSERT(textureSampler != vk::Sampler{}, "Failed to create sampler");
}

void Application::createVertexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	BufferProperties properties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferSrc,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};

	auto stagingBuffer = Buffer(state, properties);
	auto data = state.device.mapMemory(stagingBuffer.memory, 0, bufferSize);

	std::memcpy(data, vertices.data(), bufferSize);

	state.device.unmapMemory(stagingBuffer.memory);

	BufferProperties vertexBufferProperties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};

	vertexBuffer = Buffer(state, vertexBufferProperties);

	copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, bufferSize);

	state.device.destroyBuffer(stagingBuffer.buffer);
	state.device.freeMemory(stagingBuffer.memory);
}

void Application::createIndexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	BufferProperties properties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferSrc,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};
	auto stagingBuffer = Buffer(state, properties);

	auto data = state.device.mapMemory(stagingBuffer.memory, 0, bufferSize);

	std::memcpy(data, indices.data(), bufferSize);

	state.device.unmapMemory(stagingBuffer.memory);


	BufferProperties indexBufferProperties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};
	indexBuffer = Buffer(state, indexBufferProperties);

	copyBuffer(stagingBuffer.buffer, indexBuffer.buffer, bufferSize);

	state.device.destroyBuffer(stagingBuffer.buffer);
	state.device.freeMemory(stagingBuffer.memory);
}

void Application::createUniformBuffers()
{
	vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

	uboBuffers.resize(MaxFramesInFlight);
	BufferProperties properties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eUniformBuffer,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};

	for (auto i = 0; i < MaxFramesInFlight; i++)
	{
		uboBuffers[i] = Buffer(state, properties);
	}
}

void Application::createDescriptorPool()
{
	vk::DescriptorPoolSize poolSize{};
	poolSize.setType(vk::DescriptorType::eUniformBuffer);
	poolSize.setDescriptorCount(MaxFramesInFlight);

	vk::DescriptorPoolSize samplerPoolSize{};
	samplerPoolSize.setType(vk::DescriptorType::eCombinedImageSampler);
	samplerPoolSize.setDescriptorCount(MaxFramesInFlight);

	auto pools = { poolSize, samplerPoolSize };

	vk::DescriptorPoolCreateInfo poolInfo{};
	//poolInfo.setPoolSizeCount(1);
	poolInfo.setPoolSizes(pools);
	poolInfo.setMaxSets(MaxFramesInFlight);

	descriptorPool = state.device.createDescriptorPool(poolInfo);
	ENGINE_ASSERT(descriptorPool != vk::DescriptorPool{}, "Failed to create desriptor pool")
}

void Application::createDescriptorSets()
{
	std::vector<vk::DescriptorSetLayout> layouts(MaxFramesInFlight, descriptorSetLayout);

	vk::DescriptorSetAllocateInfo allocInfo{};
	allocInfo.setDescriptorPool(descriptorPool);
	allocInfo.setDescriptorSetCount(MaxFramesInFlight);
	allocInfo.setSetLayouts(layouts);

	descriptorSets = state.device.allocateDescriptorSets(allocInfo);
	for (const auto& descriptorSet : descriptorSets)
	{
		ENGINE_ASSERT(descriptorSet != vk::DescriptorSet{}, "Failed to create descriptor set");
	}

	for (auto i = 0; i < MaxFramesInFlight; i++)
	{
		vk::DescriptorBufferInfo bufferInfo{};
		bufferInfo.setBuffer(uboBuffers[i].buffer);
		bufferInfo.setOffset(0);
		bufferInfo.setRange(sizeof(UniformBufferObject));

		vk::DescriptorImageInfo imageInfo{};
		imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		imageInfo.setImageView(textureImage.view);
		imageInfo.setSampler(textureSampler);

		vk::WriteDescriptorSet descriptorWrite{};
		descriptorWrite.setDstSet(descriptorSets[i]);
		descriptorWrite.setDstBinding(0);
		descriptorWrite.setDstArrayElement(0);
		descriptorWrite.setDescriptorType(vk::DescriptorType::eUniformBuffer);
		descriptorWrite.setBufferInfo(bufferInfo);
		//descriptorWrite.setImageInfo(nullptr);
		//descriptorWrite.setTexelBufferView(nullptr);
		//descriptorWrite.setDescriptorCount(1);
		
		vk::WriteDescriptorSet imageDescriptorWrite{};
		imageDescriptorWrite.setDstSet(descriptorSets[i]);
		imageDescriptorWrite.setDstBinding(1);
		imageDescriptorWrite.setDstArrayElement(0);
		imageDescriptorWrite.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
		//imageDescriptorWrite.setBufferInfo(bufferInfo);
		//imageDescriptorWrite.setTexelBufferView(nullptr);
		imageDescriptorWrite.setImageInfo(imageInfo);
		//imageDescriptorWrite.setDescriptorCount(1);
		
		auto descriptorSets = { descriptorWrite, imageDescriptorWrite };
		state.device.updateDescriptorSets(descriptorSets, nullptr);
	}
}

void Application::createCommandBuffers()
{
	commandBuffers = CommandBuffer(state, MaxFramesInFlight);
}

void Application::createSyncObjects()
{
	imageAvailableSemaphores.resize(MaxFramesInFlight);
	renderFinishedSemaphores.resize(MaxFramesInFlight);
	inFlightFences.resize(MaxFramesInFlight);

	for (int i = 0; i < MaxFramesInFlight; i++)
	{
		vk::SemaphoreCreateInfo semaphoreInfo{};
		vk::FenceCreateInfo fenceInfo{};
		fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

		imageAvailableSemaphores[i] = state.device.createSemaphore(semaphoreInfo);
		ENGINE_ASSERT(imageAvailableSemaphores[i] != vk::Semaphore{}, "Failed to create semaphore");

		renderFinishedSemaphores[i] = state.device.createSemaphore(semaphoreInfo);
		ENGINE_ASSERT(renderFinishedSemaphores[i] != vk::Semaphore{}, "Failed to create semaphore");

		inFlightFences[i] = state.device.createFence(fenceInfo);
		ENGINE_ASSERT(inFlightFences[i] != vk::Fence{}, "Failed to create semaphore");
	}
}

void Application::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();

	auto time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.f), time * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	ubo.view = glm::lookAt(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
	ubo.proj = glm::perspective(glm::radians(45.f), swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 10.f);
	ubo.proj[1][1] *= -1;

	auto data = state.device.mapMemory(uboBuffers[currentImage].memory, 0, sizeof(ubo));

	std::memcpy(data, &ubo, sizeof(ubo));

	state.device.unmapMemory(uboBuffers[currentImage].memory);
}

void Application::recreateSwapChain()
{
	auto [width, height] = window->getFramebufferSize();

	while (width == 0 || height == 0)
	{
		std::tie(width, height) = window->getFramebufferSize();
		window->waitEvents();
	}
	state.device.waitIdle();

	cleanupSwapchain();

	createSwapchain();
	createDepthResources();
	createFramebuffers();
}

void Application::drawFrame() 
{
	state.device.waitForFences(inFlightFences[currentFrame], true, UINT64_MAX);

	uint32_t imageIndex = 0;
	auto result = state.device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr, &imageIndex);

	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		recreateSwapChain();
		return;
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		ENGINE_ASSERT(false, "Failed to acquire next image");
	}

	state.device.resetFences(inFlightFences[currentFrame]);

	updateUniformBuffer(currentFrame);

	auto commandBuffer = commandBuffers[currentFrame];

	commandBuffers.record(currentFrame, imageIndex,
		[&](vk::CommandBuffer commandBuffer) {
			vk::RenderPassBeginInfo renderPassInfo{};
			renderPassInfo.setRenderPass(renderPass);
			renderPassInfo.setFramebuffer(swapChainFramebuffers[imageIndex]);
			renderPassInfo.setRenderArea(vk::Rect2D({ 0, 0 }, swapchainExtent));

			vk::ClearValue clearColour;
			clearColour.color.setFloat32({ 0.f, 0.f, 0.f, 1.f });
			vk::ClearValue clearDepth;
			clearDepth.depthStencil = { {1.f, 0} };

			auto clearValues = { clearColour, clearDepth };
			renderPassInfo.setClearValues(clearValues);

			commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

			vk::DeviceSize offsets = { 0 };
			commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
			commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);
			commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSets[currentFrame], nullptr);

			vk::Viewport viewport{};
			viewport.setX(0.f);
			viewport.setY(0.f);
			viewport.setWidth(static_cast<float>(swapchainExtent.width));
			viewport.setHeight(static_cast<float>(swapchainExtent.height));
			viewport.setMinDepth(0.f);
			viewport.setMaxDepth(1.f);

			commandBuffer.setViewport(0, viewport);

			vk::Rect2D scissor{};
			scissor.setOffset({ 0,0 });
			scissor.setExtent(swapchainExtent);

			commandBuffer.setScissor(0, scissor);


			commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

			commandBuffer.endRenderPass();

		});
	//commandBuffer.reset();
	//recordCommandBuffer(commandBuffer, imageIndex);

	vk::SubmitInfo submitInfo{};
	
	vk::Semaphore waitSemaphore[] = { imageAvailableSemaphores[currentFrame]};

	vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	submitInfo.setWaitSemaphoreCount(1);
	submitInfo.setWaitSemaphores(imageAvailableSemaphores[currentFrame]);
	submitInfo.setWaitDstStageMask(waitStages);
	//submitInfo.setCommandBufferCount(1);
	submitInfo.setCommandBuffers(commandBuffer);
	
	//submitInfo.setSignalSemaphoreCount(1);
	submitInfo.setSignalSemaphores(renderFinishedSemaphores[currentFrame]);

	state.graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

	vk::PresentInfoKHR presentInfo{}; 
	//presentInfo.setWaitSemaphoreCount(1);
	presentInfo.setWaitSemaphores(renderFinishedSemaphores[currentFrame]);

	//presentInfo.setSwapchainCount(1);
	presentInfo.setSwapchains(swapchain);
	presentInfo.setImageIndices(imageIndex);
	//presentInfo.setResults(nullptr);

	result = presentQueue.presentKHR(&presentInfo);

	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		recreateSwapChain();
		return;
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		ENGINE_ASSERT(false, "Failed to acquire next image");
	}

	currentFrame = (currentFrame + 1) % MaxFramesInFlight;
}

vk::ShaderModule Application::createShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo createInfo{};

	createInfo.setCodeSize(code.size());
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	auto module = state.device.createShaderModule(createInfo);
	ENGINE_ASSERT(module != vk::ShaderModule{}, "Failed to create shader module");

	return module;
}

std::vector<char> Application::readShader(const std::filesystem::path& filePath)
{
	std::filesystem::path shaderPath;

	for (const auto& path : gShaderDirectories)
	{
		auto pathToShader = path / filePath;
		if (std::filesystem::exists(pathToShader))
		{
			pathToShader = std::filesystem::canonical(pathToShader);
			shaderPath = pathToShader;
			break;
		}
	}
	if (!std::filesystem::exists(shaderPath))
	{
		return {};
	}
	std::ifstream file(shaderPath.c_str(), std::ios::ate | std::ios::binary);

	ENGINE_ASSERT(file.is_open(), "Failed to open file");

	auto fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

std::vector<Image> Application::getSwapchainImages(vk::SwapchainKHR swapchain)
{
	std::vector<Image> swapchainImages;
	auto images = state.device.getSwapchainImagesKHR(swapchain);

	for (const auto& image : images)
	{
		swapchainImages.emplace_back(Image{ state , image, swapchainFormat });
	}

	return swapchainImages;
}

void Application::cleanup()
{

	cleanupSwapchain();
	state.device.destroySampler(textureSampler);

	textureImage.destroy();
	depthImage.destroy();

	for (int i = 0; i < MaxFramesInFlight; i++)
	{
		state.device.destroyFence(inFlightFences[i]);
		state.device.destroySemaphore(renderFinishedSemaphores[i]);
		state.device.destroySemaphore(imageAvailableSemaphores[i]);
	}
	state.device.destroyDescriptorPool(descriptorPool);
	state.device.destroyCommandPool(state.commandPool);
	
	vertexBuffer.destroy();
	indexBuffer.destroy();
	for (int i = 0; i < MaxFramesInFlight; i++)
	{
		uboBuffers[i].destroy();
	}

	state.device.destroyDescriptorSetLayout(descriptorSetLayout);
	state.device.destroyPipeline(graphicsPipeline);
	state.device.destroyPipelineLayout(pipelineLayout);
	state.device.destroyRenderPass(renderPass);
	state.device.destroyShaderModule(fragShaderModule);
	state.device.destroyShaderModule(vertShaderModule);

	state.device.destroy();
	state.instance.destroySurfaceKHR(surface);
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(state.instance, debugMessenger, nullptr);
	}
	state.instance.destroy();
}

void Application::cleanupSwapchain()
{
	for (auto framebuffer : swapChainFramebuffers)
	{
		state.device.destroyFramebuffer(framebuffer);
	}

	for (auto& image : swapchainImages)
	{
		image.destroyView();
	}

	state.device.destroySwapchainKHR(swapchain);
}

