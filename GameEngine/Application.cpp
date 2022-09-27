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
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createCommandBuffer();
	createSyncObjects();
}

void Application::mainLoop() const
{
	while (!window->isRunning())
	{
		window->OnUpdate();
		drawFrame();
	}

	device.waitIdle();
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


	instance = vk::createInstance(createInfo);
}

void Application::createSurface()
{
	surface = window->createSurface(instance);
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
	deviceCreateInfo.setQueueCreateInfoCount((uint32_t)deviceQueueCreateInfos.size());
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
		return;
	} 

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
	{
		ENGINE_ASSERT("{0}", "Failed to set up debug messenger");
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

	vk::SubpassDescription subpass{};
	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
	subpass.setColorAttachmentCount(1);
	subpass.setPColorAttachments(&colourAttachmentRef);

	vk::SubpassDependency dependency{};
	dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
	dependency.setDstSubpass(0);
	dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
//	dependency.srcAccessMask = 0;
	dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo renderPassInfo{};
	renderPassInfo.setAttachmentCount(1);
	renderPassInfo.setPAttachments(&colourAttachment);
	renderPassInfo.setSubpassCount(1);
	renderPassInfo.setPSubpasses(&subpass);
	renderPassInfo.setDependencyCount(1);
	renderPassInfo.setDependencies(dependency);

	renderPass = device.createRenderPass(renderPassInfo);

	if (renderPass == vk::RenderPass{})
	{
		ENGINE_ASSERT(false, "Failed to create render pass");
	}
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

	dynamicStateCreateInfo.setDynamicStateCount(dynamicStates.size());
	dynamicStateCreateInfo.setPDynamicStates(dynamicStates.data());

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.setVertexBindingDescriptionCount(0);
	vertexInputInfo.setVertexBindingDescriptions(nullptr);
	vertexInputInfo.setVertexAttributeDescriptionCount(0);
	vertexInputInfo.setVertexAttributeDescriptions(nullptr);

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
	viewportState.setViewportCount(1);
	viewportState.setPViewports(&viewport);
	viewportState.setScissorCount(1);
	viewportState.setPScissors(&scissor);

	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.setDepthClampEnable(false);
	rasterizer.setRasterizerDiscardEnable(false);
	rasterizer.setPolygonMode(vk::PolygonMode::eFill);
	rasterizer.setLineWidth(1.f);
	rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
	rasterizer.setFrontFace(vk::FrontFace::eClockwise);

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
	colourBlenderAttachment.setBlendEnable(true);
	colourBlenderAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
	colourBlenderAttachment.setDstColorBlendFactor(vk::BlendFactor::eSrc1Alpha);
	colourBlenderAttachment.setColorBlendOp(vk::BlendOp::eAdd);
	colourBlenderAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
	colourBlenderAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
	colourBlenderAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

	vk::PipelineColorBlendStateCreateInfo colourBlending{};
	colourBlending.setLogicOpEnable(false);
	colourBlending.setLogicOp(vk::LogicOp::eCopy);
	colourBlending.setAttachmentCount(1);
	colourBlending.setPAttachments(&colourBlenderAttachment);
	colourBlending.setBlendConstants({ 0.f, 0.f, 0.f, 0.f });


	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.setPushConstantRangeCount(0);
	pipelineLayoutInfo.setPPushConstantRanges(nullptr);


	pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

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

	pipelineInfo.setLayout(pipelineLayout);
	pipelineInfo.setRenderPass(renderPass);
	pipelineInfo.setSubpass(0);
	pipelineInfo.setBasePipelineIndex(VK_NULL_HANDLE);
	pipelineInfo.setBasePipelineIndex(-1);

	vk::PipelineCache cache{};

	auto success = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(&pipelineInfo), nullptr, reinterpret_cast<VkPipeline*>(&graphicsPipeline));
	if (success != VK_SUCCESS)
	{
		ENGINE_ASSERT(false, "Failed to create graphics pipeline");
	}
}

void Application::createFramebuffers()
{
	for (const auto& swapchainImageView : swapchainImageViews)
	{
		vk::ImageView attachments[] =
		{
			swapchainImageView
		};

		vk::FramebufferCreateInfo framebufferInfo{};
		framebufferInfo.setRenderPass(renderPass);
		framebufferInfo.setAttachmentCount(1);
		framebufferInfo.setPAttachments(attachments);
		framebufferInfo.setWidth(swapchainExtent.width);
		framebufferInfo.setHeight(swapchainExtent.height);
		framebufferInfo.setLayers(1);

		swapChainFramebuffers.emplace_back(device.createFramebuffer(framebufferInfo));
	}

}

void Application::createCommandPool()
{
	auto queuFamilyIndices = findQueueFamiles(physicalDevice);

	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	poolInfo.setQueueFamilyIndex(queuFamilyIndices.graphicsFamily.value());

	commandPool = device.createCommandPool(poolInfo);
}

void Application::createCommandBuffer()
{
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.setCommandPool(commandPool);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandBufferCount(1);

	commandBuffer = device.allocateCommandBuffers(allocInfo)[0];
	if (commandBuffer == vk::CommandBuffer{})
	{
		ENGINE_ASSERT(false, "Failed to create command buffer");
	}
}

void Application::createSyncObjects()
{
	vk::SemaphoreCreateInfo semaphoreInfo{};
	vk::FenceCreateInfo fenceInfo{};
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

	imageAvailableSemaphore = device.createSemaphore(semaphoreInfo);
	renderFinishedSemaphore = device.createSemaphore(semaphoreInfo);
	inFlightFence = device.createFence(fenceInfo);


}

void Application::recordCommandBuffer(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex) const
{
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.setPInheritanceInfo(nullptr);

	commandBuffer.begin(beginInfo);

	vk::RenderPassBeginInfo renderPassInfo{};
	renderPassInfo.setRenderPass(renderPass);
	renderPassInfo.setFramebuffer(swapChainFramebuffers[imageIndex]);
	renderPassInfo.setRenderArea(vk::Rect2D({ 0, 0 }, swapchainExtent));

	vk::ClearColorValue clearValue;
	clearValue.setFloat32({ 0.f, 0.f, 0.f, 1.f });
	vk::ClearValue clearColour{ clearValue};
	renderPassInfo.setClearValueCount(1);
	renderPassInfo.setPClearValues(&clearColour);

	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

	vk::Viewport viewport{};
	viewport.setX(0.f);
	viewport.setY(0.f);
	viewport.setWidth(swapchainExtent.width);
	viewport.setHeight(swapchainExtent.height);
	viewport.setMinDepth(0.f);
	viewport.setMaxDepth(1.f);

	commandBuffer.setViewport(0, viewport);

	vk::Rect2D scissor{};
	scissor.setOffset({ 0,0 });
	scissor.setExtent(swapchainExtent);

	commandBuffer.setScissor(0, scissor);


	commandBuffer.draw(3, 1, 0, 0);

	commandBuffer.endRenderPass();
	commandBuffer.end();
}

void Application::drawFrame() const
{
	device.waitForFences(inFlightFence, true, UINT64_MAX);
	device.resetFences(inFlightFence);

	uint32_t imageIndex = device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphore, nullptr);
	commandBuffer.reset();
	recordCommandBuffer(commandBuffer, imageIndex);

	vk::SubmitInfo submitInfo{};
	
	vk::Semaphore waitSemaphore[] = { imageAvailableSemaphore };

	vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	submitInfo.setWaitSemaphoreCount(1);
	submitInfo.setWaitSemaphores(imageAvailableSemaphore);
	submitInfo.setWaitDstStageMask(waitStages);
	submitInfo.setCommandBufferCount(1);
	submitInfo.setCommandBuffers(commandBuffer);
	
	submitInfo.setSignalSemaphoreCount(1);
	submitInfo.setSignalSemaphores(renderFinishedSemaphore);

	graphicsQueue.submit(submitInfo, inFlightFence);

	vk::PresentInfoKHR presentInfo{}; 
	presentInfo.setWaitSemaphoreCount(1);
	presentInfo.setWaitSemaphores(renderFinishedSemaphore);

	//presentInfo.setSwapchainCount(1);
	presentInfo.setSwapchains(swapchain);
	presentInfo.setImageIndices(imageIndex);
	//presentInfo.setResults(nullptr);

	presentQueue.presentKHR(presentInfo);
}

vk::ShaderModule Application::createShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo createInfo{};

	createInfo.setCodeSize(code.size());
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	return device.createShaderModule(createInfo);
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

	if (!file.is_open())
	{
		ENGINE_ASSERT(false, "Failed to open file");
	}

	auto fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	return buffer;
}

void Application::cleanup()
{
	device.destroyFence(inFlightFence);
	device.destroySemaphore(renderFinishedSemaphore);
	device.destroySemaphore(imageAvailableSemaphore);
	device.destroyCommandPool(commandPool);
	for (auto framebuffer : swapChainFramebuffers)
	{
		device.destroyFramebuffer(framebuffer);
	}

	device.destroyPipeline(graphicsPipeline);
	device.destroyPipelineLayout(pipelineLayout);
	device.destroyRenderPass(renderPass);
	device.destroyShaderModule(fragShaderModule);
	device.destroyShaderModule(vertShaderModule);
	for (auto& imageView : swapchainImageViews)
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

