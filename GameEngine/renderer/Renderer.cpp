#include "Renderer.h"
#include "../EngineCore.h"
#include "../Application.h"

#include <set>
#include <string>
#include <fstream>


void Renderer::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createTextureSampler();
	createSwapchain();
	createRenderPass();
	createDescriptorSetLayout();
	createCommandPool();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createDepthResources();
	createFramebuffers();
	createVertexBuffer();
	createIndexBuffer();
	//updateDescriptorSets();
	createCommandBuffers();
	createSyncObjects();
	createGraphicsPipeline();
}


void Renderer::create()
{
	initVulkan();
	_gui = Gui::get();
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void*)
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
		//	LOG_ERROR("{0}: {1}", "Debug layer", callbackData->pMessage);
	}

	return true;
}


bool Renderer::isDeviceSuitable(const vk::PhysicalDevice& device)
{
	auto queueIndices = findQueueFamiles(device);

	auto properties = device.getProperties();
	auto features = device.getFeatures();

	auto swapChainSupport = querySwapChainSupport(device);
	auto swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

	return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && features.geometryShader && queueIndices.isComplete() && swapChainAdequate;
}

bool Renderer::checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice)
{
	auto extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : extensionProperties)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

Renderer::SwapChainSupportDetails Renderer::querySwapChainSupport(const vk::PhysicalDevice& physicalDevice)
{
	SwapChainSupportDetails details;

	details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
	details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

	return details;
}

vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
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

vk::PresentModeKHR Renderer::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
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

vk::Extent2D Renderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		const auto& app = Application::get();

		auto [width, height] = app.window()->getFramebufferSize();
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

std::vector<Texture2D> Renderer::getSwapchainImages(vk::SwapchainKHR swapchain)
{
	std::vector<Texture2D> swapchainImages;
	auto images = state.device.getSwapchainImagesKHR(swapchain);

	for (const auto& image : images)
	{
		swapchainImages.emplace_back(Texture2D{ image, _swapchainFormat, textureSampler });
	}

	return swapchainImages;
}


Renderer::QueueFamilyIndices Renderer::findQueueFamiles(const vk::PhysicalDevice& device)
{
	QueueFamilyIndices queueIndices;
	auto familyProperties = device.getQueueFamilyProperties();
	int i = 0;
	for (const auto& queueFamily : familyProperties)
	{
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			queueIndices.graphicsFamily = i;
		}
		auto presentSupport = device.getSurfaceSupportKHR(i, surface);

		if (presentSupport)
		{
			queueIndices.presentFamily = i;
		}

		if (queueIndices.isComplete())
		{
			break;
		}

		i++;
	}
	return queueIndices;
}

bool Renderer::hasStencilComponent(vk::Format format)
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void Renderer::setupDebugMessenger()
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

	if (CreateDebugUtilsMessengerEXT(state.instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
	{
		ENGINE_ASSERT(false, "Failed to set up debug messenger");
	}
}

VkResult Renderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger)
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

void Renderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, allocator);
	}
}

void Renderer::createInstance()
{
	vk::ApplicationInfo info{};
	info
		.setPApplicationName("Hello Triangle")
		.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
		.setPEngineName("NoEngine")
		.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
		.setApiVersion(VK_API_VERSION_1_0);

	vk::InstanceCreateInfo createInfo{};
	createInfo.setPApplicationInfo(&info);

	const auto& app = Application::get();

	auto extensions = app.window()->GetRequiredExtensions();

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

void Renderer::createSurface()
{
	const auto& app = Application::get();
	surface = app.window()->createSurface(state.instance);
}

void Renderer::pickPhysicalDevice()
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

void Renderer::createLogicalDevice()
{
	auto queueIndices = findQueueFamiles(state.physicalDevice);

	std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
	std::set<uint32_t> uniqueQueueFamiles = { queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value() };

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

	state.device = state.physicalDevice.createDevice(deviceCreateInfo/*, state.allocator*/);

	ENGINE_ASSERT(state.device != vk::Device{}, "Failed to create logical device");

	state.graphicsQueue = state.device.getQueue(queueIndices.graphicsFamily.value(), 0);
	state.presentQueue = state.device.getQueue(queueIndices.presentFamily.value(), 0);

	state.queueFamily = queueIndices.graphicsFamily.value();
}

bool Renderer::checkValidationLayerSupport()
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

void Renderer::createSwapchain()
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
	createInfo
		.setSurface(surface)
	    .setMinImageCount(imageCount)
	    .setImageFormat(surfaceFormat.format)
	    .setImageColorSpace(surfaceFormat.colorSpace)
	    .setImageExtent(extent)
	    .setImageArrayLayers(1)
	    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

	auto queueIndices = findQueueFamiles(state.physicalDevice);

	uint32_t queueFamilyIndices[] = { queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value() };

	if (queueIndices.graphicsFamily != queueIndices.presentFamily)
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

	_swapchain = state.device.createSwapchainKHR(createInfo);

	ENGINE_ASSERT(_swapchain != vk::SwapchainKHR{}, "Failed to create swap chain");

	_swapchainFormat = surfaceFormat.format;
	_swapchainImages = getSwapchainImages(_swapchain);
	ENGINE_ASSERT(!_swapchainImages.empty(), "Failed to retrieve swapchain images");
	swapchainExtent = extent;
}

void Renderer::createRenderPass()
{
	vk::AttachmentDescription colourAttachment{};
	colourAttachment.setFormat(_swapchainFormat);
	colourAttachment.setSamples(vk::SampleCountFlagBits::e1);
	colourAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
	colourAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
	colourAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
	colourAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
	colourAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
	colourAttachment.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

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

void Renderer::createDescriptorSetLayout()
{
	//vk::DescriptorSetLayoutBinding uboLayoutBinding{};
	//uboLayoutBinding
	//	.setBinding(0)
	//	.setDescriptorCount(1)
	//	.setDescriptorType(vk::DescriptorType::eUniformBuffer)
	//	.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding
		.setBinding(1)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment);

	vk::DescriptorSetLayoutBinding fragUboBinding{};
	fragUboBinding
		.setBinding(0)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment);

	vk::DescriptorSetLayoutBinding cameraUboBinding{};
	cameraUboBinding
		.setBinding(2)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	auto bindings = { /*uboLayoutBinding,*/ samplerLayoutBinding, fragUboBinding, cameraUboBinding };
	vk::DescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.setBindings(bindings);

	descriptorSetLayout = state.device.createDescriptorSetLayout(layoutInfo/*, state.allocator*/);
	ENGINE_ASSERT(descriptorSetLayout != vk::DescriptorSetLayout{}, "Failed to create descriptor set");
}

void Renderer::createGraphicsPipeline()
{
	auto boardVertShaderCode = readShader("boardVert.spv");
	auto boardFragShaderCode = readShader("boardFrag.spv");

	auto vertShaderModule = createShaderModule(boardVertShaderCode);
	auto fragShaderModule = createShaderModule(boardFragShaderCode);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setModule(vertShaderModule)
		.setPName("main");

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(fragShaderModule)
		.setPName("main");

	auto shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

	auto dynamicStates =
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{};

	dynamicStateCreateInfo.setDynamicStates(dynamicStates);

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescription = Vertex::getAttributeDescription();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo
		.setVertexBindingDescriptions(bindingDescription)
		.setVertexAttributeDescriptions(attributeDescription);

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly
		.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(false);

	vk::Viewport viewport{};
	viewport
		.setX(0.f)
		.setY(0.f)
		.setWidth((float)swapchainExtent.width)
		.setHeight((float)swapchainExtent.height)
		.setMinDepth(0.f)
		.setMaxDepth(1.f);

	vk::Rect2D scissor{};
	scissor
		.setOffset({ 0, 0 })
		.setExtent(swapchainExtent);

	vk::PipelineViewportStateCreateInfo viewportState{};
	viewportState
		.setViewports(viewport)
		.setScissors(scissor);

	vk::PipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer
		.setDepthClampEnable(false)
		.setRasterizerDiscardEnable(false)
		.setPolygonMode(vk::PolygonMode::eFill)
		.setLineWidth(1.f)
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setDepthBiasEnable(false)
		.setDepthBiasConstantFactor(0.f)
		.setDepthBiasClamp(0.f)
		.setDepthBiasSlopeFactor(0.f);

	vk::PipelineMultisampleStateCreateInfo multiSample{};

	multiSample
		.setSampleShadingEnable(false)
		.setRasterizationSamples(vk::SampleCountFlagBits::e1)
		.setMinSampleShading(1.f)
		.setPSampleMask(nullptr)
		.setAlphaToCoverageEnable(false)
		.setAlphaToOneEnable(false);

	vk::PipelineColorBlendAttachmentState colourBlenderAttachment{};
	colourBlenderAttachment
		.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
		.setBlendEnable(false)
		.setSrcColorBlendFactor(vk::BlendFactor::eOne)
		.setDstColorBlendFactor(vk::BlendFactor::eOne)
		.setColorBlendOp(vk::BlendOp::eAdd)
		.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
		.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
		.setAlphaBlendOp(vk::BlendOp::eAdd);

	vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};
	depthStencilInfo
		.setDepthTestEnable(true)
		.setDepthWriteEnable(true)
		.setDepthCompareOp(vk::CompareOp::eLess)
		.setDepthBoundsTestEnable(false)
		.setStencilTestEnable(false);


	vk::PipelineColorBlendStateCreateInfo colourBlending{};
	colourBlending
		.setLogicOpEnable(false)
		.setLogicOp(vk::LogicOp::eCopy)
		.setAttachments(colourBlenderAttachment)
		.setBlendConstants({ 0.f, 0.f, 0.f, 0.f });


	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo
		.setPushConstantRanges(nullptr)
		.setSetLayouts(descriptorSetLayout);

	pipelineLayout = state.device.createPipelineLayout(pipelineLayoutInfo);
	ENGINE_ASSERT(pipelineLayout != vk::PipelineLayout{}, "FAiled to create pipeline layout");

	vk::GraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo
		.setStages(shaderStages)
		.setPVertexInputState(&vertexInputInfo)
		.setPInputAssemblyState(&inputAssembly)
		.setPViewportState(&viewportState)
		.setPRasterizationState(&rasterizer)
		.setPMultisampleState(&multiSample)
		.setPColorBlendState(&colourBlending)
		.setPDynamicState(&dynamicStateCreateInfo)
		.setPDepthStencilState(&depthStencilInfo)
		.setLayout(pipelineLayout)
		.setRenderPass(renderPass)
		.setSubpass(0)
	//pipelineInfo.setBasePipelineHandle(VK_NULL_HANDLE);
		.setBasePipelineIndex(-1);



	auto createInfos = { pipelineInfo };

	auto pipelin = state.device.createGraphicsPipelines(VK_NULL_HANDLE, createInfos).value;

	pipelines.board = pipelin[0];
	ENGINE_ASSERT(pipelines.board != vk::Pipeline{}, "Failed to create graphics pipeline");
	//ENGINE_ASSERT(pipelines.pieces != vk::Pipeline{}, "Failed to create graphics pipeline");

	SpriteRenderer::create(pipelineInfo, descriptorSets.pieces);

}

vk::ShaderModule Renderer::createShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo createInfo{};

	createInfo.setCodeSize(code.size());
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	auto module = state.device.createShaderModule(createInfo);
	ENGINE_ASSERT(module != vk::ShaderModule{}, "Failed to create shader module");

	return module;
}

std::vector<char> Renderer::readShader(const std::filesystem::path& filePath)
{
	std::filesystem::path shaderPath;

	auto shaderDirectories = Application::getShaderDirectories();

	for (const auto& path : shaderDirectories)
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

void Renderer::registerImage(const Texture2D& texture)
{
	_images.push_back(texture);
	updateDescriptorSets();
}


void Renderer::createFramebuffers()
{
	swapChainFramebuffers.resize(0);
	for (const auto& image : _swapchainImages)
	{
		const auto attachments =
		{
			image.view,
			depthImage.view
		};

		vk::FramebufferCreateInfo framebufferInfo{};
		framebufferInfo
			.setRenderPass(renderPass)
			.setAttachments(attachments)
			.setWidth(swapchainExtent.width)
			.setHeight(swapchainExtent.height)
			.setLayers(1);

		auto framebuffer = state.device.createFramebuffer(framebufferInfo);
		ENGINE_ASSERT(framebuffer != vk::Framebuffer{}, "Failed to create framebuffer");
		swapChainFramebuffers.emplace_back(framebuffer);
	}
}

void Renderer::createCommandPool()
{
	auto queuFamilyIndices = findQueueFamiles(state.physicalDevice);

	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	poolInfo.setQueueFamilyIndex(queuFamilyIndices.graphicsFamily.value());

	state.commandPool = state.device.createCommandPool(poolInfo);
	ENGINE_ASSERT(state.commandPool != vk::CommandPool{}, "FAiled to create command pool");
}

void Renderer::createDepthResources()
{
	auto format = state.findDepthFormat();
	TextureProperties properties
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

void Renderer::createTextureSampler()
{
	vk::SamplerCreateInfo samplerInfo{};
	samplerInfo
		.setMagFilter(vk::Filter::eNearest)
		.setMinFilter(vk::Filter::eNearest)
		.setAddressModeU(vk::SamplerAddressMode::eRepeat)
		.setAddressModeV(vk::SamplerAddressMode::eRepeat)
		.setAddressModeW(vk::SamplerAddressMode::eRepeat)
		.setAnisotropyEnable(true);

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

void Renderer::createVertexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	BufferProperties properties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferSrc,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};

	auto stagingBuffer = Buffer(properties);
	auto data = state.device.mapMemory(stagingBuffer.memory, 0, bufferSize);

	std::memcpy(data, vertices.data(), bufferSize);

	state.device.unmapMemory(stagingBuffer.memory);

	BufferProperties vertexBufferProperties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};

	vertexBuffer = Buffer(vertexBufferProperties);

	copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, bufferSize);

	state.device.destroyBuffer(stagingBuffer.buffer);
	state.device.freeMemory(stagingBuffer.memory);
}

void Renderer::createIndexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	BufferProperties properties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferSrc,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};
	auto stagingBuffer = Buffer(properties);

	auto data = state.device.mapMemory(stagingBuffer.memory, 0, bufferSize);

	std::memcpy(data, indices.data(), bufferSize);

	state.device.unmapMemory(stagingBuffer.memory);


	BufferProperties indexBufferProperties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};
	indexBuffer = Buffer(indexBufferProperties);

	copyBuffer(stagingBuffer.buffer, indexBuffer.buffer, bufferSize);

	state.device.destroyBuffer(stagingBuffer.buffer);
	state.device.freeMemory(stagingBuffer.memory);
}

void Renderer::createUniformBuffers()
{
	vk::DeviceSize bufferSize = sizeof(BoardProperties);

	//uboBuffers.resize(MaxFramesInFlight);
	cameraBuffer.resize(MaxFramesInFlight);
	boardPropertiesBuffer.resize(MaxFramesInFlight);
	BufferProperties properties =
	{
		.size = bufferSize,
		.usage = vk::BufferUsageFlagBits::eUniformBuffer,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};

	BufferProperties cameraProperties =
	{
		.size = sizeof(glm::mat4),
		.usage = vk::BufferUsageFlagBits::eUniformBuffer,
		.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
	};

	for (auto i = 0; i < MaxFramesInFlight; i++)
	{
		//uboBuffers[i] = Buffer(state, properties);
		cameraBuffer[i] = Buffer(cameraProperties);
		boardPropertiesBuffer[i] = Buffer(properties);
	}
}

void Renderer::createDescriptorPool()
{
	const uint32_t descriptorCount = 1000;

#define DESCRIPTOR_POOL(name, type)			\
	vk::DescriptorPoolSize name{};			\
	name									\
		.setType(type) 						\
		.setDescriptorCount(descriptorCount)


	DESCRIPTOR_POOL(sampler, vk::DescriptorType::eSampler);
	DESCRIPTOR_POOL(combinedSample, vk::DescriptorType::eCombinedImageSampler);
	DESCRIPTOR_POOL(sampled, vk::DescriptorType::eSampledImage);
	DESCRIPTOR_POOL(storageImage, vk::DescriptorType::eStorageImage);
	DESCRIPTOR_POOL(uniformTexel, vk::DescriptorType::eUniformTexelBuffer);
	DESCRIPTOR_POOL(storageTexel, vk::DescriptorType::eStorageTexelBuffer);
	DESCRIPTOR_POOL(uniform, vk::DescriptorType::eUniformBuffer);
	DESCRIPTOR_POOL(storage, vk::DescriptorType::eStorageBuffer);
	DESCRIPTOR_POOL(uniformDynamic, vk::DescriptorType::eUniformBufferDynamic);
	DESCRIPTOR_POOL(storageDynamic, vk::DescriptorType::eStorageBufferDynamic);
	DESCRIPTOR_POOL(input, vk::DescriptorType::eInputAttachment);

	auto descriptorPools = { uniform, combinedSample };

	vk::DescriptorPoolCreateInfo info{};
	info.setPoolSizes(descriptorPools);
	info.setMaxSets(MaxFramesInFlight * 2);
	descriptorPool = state.device.createDescriptorPool(info);
	ENGINE_ASSERT(descriptorPool != vk::DescriptorPool{}, "Failed to create desriptor pool")
}

void Renderer::createDescriptorSets()
{
	std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
	std::vector<vk::DescriptorSetLayout> layouts(MaxFramesInFlight, descriptorSetLayout);

	vk::DescriptorSetAllocateInfo allocInfo{};
	allocInfo
		.setDescriptorPool(descriptorPool)
		.setDescriptorSetCount(MaxFramesInFlight)
		.setSetLayouts(layouts);

	descriptorSets.board = state.device.allocateDescriptorSets(allocInfo);
	for (const auto& descriptorSet : descriptorSets.board)
	{
		ENGINE_ASSERT(descriptorSet != vk::DescriptorSet{}, "Failed to create descriptor set");
	}

	for (int i = 0; i < MaxFramesInFlight; i++)
	{
		boardPropertiesBuffer[i]._descriptor.range = sizeof(BoardProperties);
		vk::WriteDescriptorSet boardUniformDescriptorSet{};
		boardUniformDescriptorSet
			.setDstSet(descriptorSets.board[i])
			.setDstBinding(0)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setBufferInfo(boardPropertiesBuffer[i]._descriptor);

		writeDescriptorSets.push_back(boardUniformDescriptorSet);
	}
	state.device.updateDescriptorSets(writeDescriptorSets, nullptr);
	 
	descriptorSets.pieces = state.device.allocateDescriptorSets(allocInfo);
	for (const auto& descriptorSet : descriptorSets.pieces)
	{
		ENGINE_ASSERT(descriptorSet != vk::DescriptorSet{}, "Failed to create descriptor set");
	}


}

void Renderer::updateDescriptorSets()
{
		std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

		writeDescriptorSets.resize(0);

		for (int i = 0; i < MaxFramesInFlight; i++)
		{

			vk::WriteDescriptorSet piecesDescriptorSet{};
			piecesDescriptorSet
				.setDstSet(descriptorSets.pieces[i])
				.setDstBinding(1)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setImageInfo(_images[0]._imageInfo);

			writeDescriptorSets.push_back(piecesDescriptorSet);

			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo
				.setBuffer(cameraBuffer[i].buffer)
				.setOffset(0)
				.setRange(sizeof(glm::mat4));

			vk::WriteDescriptorSet descriptorWrite{};
			descriptorWrite
				.setDstSet(descriptorSets.pieces[i])
				.setDstBinding(2)
				.setDstArrayElement(0)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setBufferInfo(bufferInfo);

			writeDescriptorSets.push_back(descriptorWrite);
		}

		state.device.updateDescriptorSets(writeDescriptorSets, nullptr);
}


void Renderer::copyBuffer(const vk::Buffer& srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	OneTimeCommandBuffer(
		[&](vk::CommandBuffer commandBuffer)
		{
			vk::BufferCopy copyRegion{};
			copyRegion
				.setSrcOffset(0)
				.setDstOffset(0)
				.setSize(size);

			commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);
		});
}

void Renderer::createCommandBuffers()
{
	_commandBuffers = CommandBuffer(MaxFramesInFlight);
}

void Renderer::createSyncObjects()
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

void Renderer::createSprite()
{
	SpriteRenderer::createSprite({ 200, 200 }, _images[0]);
}

void Renderer::updateUniformBuffer(BoardProperties& boardProperties, uint32_t currentImage)
{
	//static auto startTime = std::chrono::high_resolution_clock::now();

	//auto currentTime = std::chrono::high_resolution_clock::now();

	//auto time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	//UniformBufferObject ubo{};
	//ubo.model = glm::rotate(glm::mat4(1.f), time * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
	//ubo.view = glm::lookAt(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
	//ubo.proj = glm::perspective(glm::radians(45.f), swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 10.f);
	//ubo.proj[1][1] *= -1;

	//auto data = state.device.mapMemory(uboBuffers[currentImage].memory, 0, sizeof(ubo));

	//std::memcpy(data, &ubo, sizeof(ubo));

	//state.device.unmapMemory(uboBuffers[currentImage].memory);

	auto ortho = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);

	auto data = state.device.mapMemory(cameraBuffer[currentImage].memory, 0, sizeof(glm::mat4));
	std::memcpy(data, &ortho, sizeof(glm::mat4));
	state.device.unmapMemory(cameraBuffer[currentImage].memory);

	auto fragData = state.device.mapMemory(boardPropertiesBuffer[currentImage].memory, 0, sizeof(BoardProperties));

	std::memcpy(fragData, &boardProperties, sizeof(boardProperties));

	state.device.unmapMemory(boardPropertiesBuffer[currentImage].memory);
}

void Renderer::drawFrame(BoardProperties& boardProperties)
{
	auto result = state.device.waitForFences(inFlightFences[currentFrame], true, UINT64_MAX);
	ENGINE_ASSERT(result == vk::Result::eSuccess, "Failed to get fence");

	uint32_t imageIndex = 0;
	result = state.device.acquireNextImageKHR(_swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr, &imageIndex);

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

	updateUniformBuffer(boardProperties, currentFrame);

	auto commandBuffer = _commandBuffers[currentFrame];


	_commandBuffers.record(currentFrame, imageIndex,
		[&](vk::CommandBuffer commandBuffer, uint32_t imageIndex)
		{
			vk::RenderPassBeginInfo renderPassInfo{};
	renderPassInfo
		.setRenderPass(renderPass)
		.setFramebuffer(swapChainFramebuffers[imageIndex])
		.setRenderArea(vk::Rect2D({ 0, 0 }, swapchainExtent));

	vk::ClearValue clearColour;
	clearColour.color.setFloat32({ clearColor.x, clearColor.y, clearColor.z, clearColor.w });
	vk::ClearValue clearDepth;
	clearDepth.depthStencil = { {1.f, 0} };

	auto clearValues = { clearColour, clearDepth };
	renderPassInfo.setClearValues(clearValues);

	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.board);

	vk::DeviceSize offsets = { 0 };
	commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
	commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSets.board[currentFrame], nullptr);


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


	commandBuffer.drawIndexed(6, 1, 0, 0, 0);

	SpriteRenderer::renderSprite(commandBuffer, currentFrame);

	commandBuffer.endRenderPass();
		});

	_gui->render(swapchainExtent, currentFrame, imageIndex);

	vk::SubmitInfo submitInfo{};

	auto waitSemaphore = { imageAvailableSemaphores[currentFrame] };

	vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	submitInfo.setWaitSemaphores(waitSemaphore);
	submitInfo.setWaitDstStageMask(waitStages);

	auto commandBuffers = { commandBuffer, _gui->getCommandBuffer(currentFrame) };
	submitInfo.setCommandBuffers(commandBuffers);

	//submitInfo.setSignalSemaphoreCount(1);
	submitInfo.setSignalSemaphores(renderFinishedSemaphores[currentFrame]);

	state.graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

	vk::PresentInfoKHR presentInfo{};
	//presentInfo.setWaitSemaphoreCount(1);
	presentInfo.setWaitSemaphores(renderFinishedSemaphores[currentFrame]);

	//presentInfo.setSwapchainCount(1);
	presentInfo.setSwapchains(_swapchain);
	presentInfo.setImageIndices(imageIndex);
	//presentInfo.setResults(nullptr);

	result = state.presentQueue.presentKHR(&presentInfo);

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

void Renderer::recreateSwapChain()
{
	const auto& app = Application::get();
	const auto& window = app.window();

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


	_gui->onWindowResize(width, height, _swapchainImages);
}

void Renderer::cleanupSwapchain()
{
	for (auto& framebuffer : swapChainFramebuffers)
	{
		state.device.destroyFramebuffer(framebuffer);
	}

	for (auto& image : _swapchainImages)
	{
		image.destroyView();
	}

	state.device.destroySwapchainKHR(_swapchain);
}

void Renderer::cleanup()
{
	cleanupSwapchain();
	state.device.destroySampler(textureSampler);

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
		//uboBuffers[i].destroy();
		cameraBuffer[i].destroy();
		boardPropertiesBuffer[i].destroy();
	}

	state.device.destroyDescriptorSetLayout(descriptorSetLayout);
	state.device.destroyPipeline(pipelines.board);
	state.device.destroyPipelineLayout(pipelineLayout);
	state.device.destroyRenderPass(renderPass);
	state.device.destroyShaderModule(fragShaderModule);
	state.device.destroyShaderModule(vertShaderModule);

	state.instance.destroySurfaceKHR(surface);
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(state.instance, _debugMessenger, nullptr);
	}
	state.instance.destroy();
}