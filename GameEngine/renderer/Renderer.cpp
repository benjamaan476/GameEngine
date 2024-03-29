#include "Renderer.h"
#include "../EngineCore.h"
#include "../Application.h"
#include "Pipeline.h"

#include "Initialisers.h"

#include <set>
#include <string>
#include <fstream>

namespace egkr
{

	void egakeru::initVulkan()
	{
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createTextureSampler();
		createSwapchain();
		createRenderPass();
		createCommandPool();
		createUniformBuffers();
		setup_descriptors();
		createDepthResources();
		createFramebuffers();
		createVertexBuffer();
		createIndexBuffer();
		//updateDescriptorSets();
		createCommandBuffers();
		createSyncObjects();
		createGraphicsPipeline();
	}


	void egakeru::create()
	{
		initVulkan();
		_gui = Gui::get();
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL egakeru::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void*)
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


	bool egakeru::isDeviceSuitable(const vk::PhysicalDevice& device)
	{
		auto queueIndices = findQueueFamiles(device);

		auto properties = device.getProperties();
		auto features = device.getFeatures();

		auto swapChainSupport = querySwapChainSupport(device);
		auto swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

		return (properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu || properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)  && features.geometryShader && queueIndices.isComplete() && swapChainAdequate;
	}

	bool egakeru::checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice)
	{
		auto extensionProperties = physicalDevice.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : extensionProperties)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	egakeru::SwapChainSupportDetails egakeru::querySwapChainSupport(const vk::PhysicalDevice& physicalDevice)
	{
		SwapChainSupportDetails details;

		details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
		details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
		details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

		return details;
	}

	vk::SurfaceFormatKHR egakeru::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
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

	vk::PresentModeKHR egakeru::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
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

	vk::Extent2D egakeru::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
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

	std::vector<Texture2D> egakeru::getSwapchainImages(vk::SwapchainKHR swapchain)
	{
		std::vector<Texture2D> swapchainImages;
		auto images = state.device.getSwapchainImagesKHR(swapchain);

		for (const auto& image : images)
		{
			swapchainImages.emplace_back(Texture2D{ image, _swapchainFormat, textureSampler });
		}

		return swapchainImages;
	}


	egakeru::QueueFamilyIndices egakeru::findQueueFamiles(const vk::PhysicalDevice& device)
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

	bool egakeru::hasStencilComponent(vk::Format format)
	{
		return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
	}

	void egakeru::setupDebugMessenger()
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

	VkResult egakeru::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger)
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

	void egakeru::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			func(instance, debugMessenger, allocator);
		}
	}

	void egakeru::createInstance()
	{
		PROFILE_FUNCTION()
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

	void egakeru::createSurface()
	{
		PROFILE_FUNCTION()
		const auto& app = Application::get();
		surface = app.window()->createSurface(state.instance);
	}

	void egakeru::pickPhysicalDevice()
	{
		PROFILE_FUNCTION()
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

	void egakeru::createLogicalDevice()
	{
		PROFILE_FUNCTION()
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

	bool egakeru::checkValidationLayerSupport()
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

	void egakeru::createSwapchain()
	{
		PROFILE_FUNCTION()
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

	void egakeru::createRenderPass()
	{
		RenderPassDescription description{
			.renderPassName = "Sample render pass"
		};
		renderPass = RenderPass::create(description);

		egkr::Attachment colourAttachment
		{
			.format = _swapchainFormat,
			.samples = vk::SampleCountFlagBits::e1,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::eColorAttachmentOptimal
		};

		renderPass->set_colour_attachment(colourAttachment);

		egkr::Attachment depthAttachment
		{
			.format = state.findDepthFormat(),
			.samples = vk::SampleCountFlagBits::e1,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eDontCare,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal
		};

		renderPass->set_depth_attachment(depthAttachment);
		renderPass->bake();
	}

	void egakeru::setup_descriptors()
	{
		DESCRIPTOR_POOL(uniform, vk::DescriptorType::eUniformBuffer);
		//DESCRIPTOR_POOL(combinedSample, vk::DescriptorType::eCombinedImageSampler);
		//DESCRIPTOR_POOL(sampler, vk::DescriptorType::eSampler);
		//DESCRIPTOR_POOL(sampled, vk::DescriptorType::eSampledImage);
		//DESCRIPTOR_POOL(storageImage, vk::DescriptorType::eStorageImage);
		//DESCRIPTOR_POOL(uniformTexel, vk::DescriptorType::eUniformTexelBuffer);
		//DESCRIPTOR_POOL(storageTexel, vk::DescriptorType::eStorageTexelBuffer);
		//DESCRIPTOR_POOL(storage, vk::DescriptorType::eStorageBuffer);
		//DESCRIPTOR_POOL(uniformDynamic, vk::DescriptorType::eUniformBufferDynamic);
		//DESCRIPTOR_POOL(storageDynamic, vk::DescriptorType::eStorageBufferDynamic);
		//DESCRIPTOR_POOL(input, vk::DescriptorType::eInputAttachment);
		
		auto descriptorPools = { uniform };
		auto poolCreateInfo = vk::DescriptorPoolCreateInfo{};
		poolCreateInfo
			.setMaxSets(MaxFramesInFlight * 2)
			.setPoolSizes(descriptorPools);
		
		//auto poolCreateInfo = initialisers::descriptors::descriptor_pool_create(descriptorPools, MaxFramesInFlight * 2);
		descriptorPool = state.device.createDescriptorPool(poolCreateInfo);
		ENGINE_ASSERT(descriptorPool != vk::DescriptorPool{}, "Failed to create desriptor pool");

		auto vbUBOLayout = initialisers::descriptors::descriptor_set_layout_binding(vk::DescriptorType::eUniformBuffer, 0, vk::ShaderStageFlagBits::eVertex, 1);

		auto bindings = { vbUBOLayout };
		//auto descriptorSetLayoutCreate = initialisers::descriptors::descriptor_set_layout_create(bindings);
		auto descriptorSetLayoutCreate = vk::DescriptorSetLayoutCreateInfo{};
		descriptorSetLayoutCreate
				.setBindings(bindings);

		descriptorSetLayout = state.device.createDescriptorSetLayout(descriptorSetLayoutCreate);
		ENGINE_ASSERT(descriptorSetLayout != vk::DescriptorSetLayout{}, "Failed to create descriptor set");

		std::vector<vk::DescriptorSetLayout> layouts(MaxFramesInFlight, descriptorSetLayout);
		//auto descriptorAllocateInfo = initialisers::descriptors::descriptor_allocate_info(descriptorPool, MaxFramesInFlight, layouts);
		auto descriptorAllocateInfo = vk::DescriptorSetAllocateInfo{};
		descriptorAllocateInfo
				.setDescriptorPool(descriptorPool)
				.setDescriptorSetCount(MaxFramesInFlight)
				.setSetLayouts(layouts);
		descriptorSets = state.device.allocateDescriptorSets(descriptorAllocateInfo);

	std::vector<vk::WriteDescriptorSet> writeDescriptors;
		for (size_t i{ 0 }; i < MaxFramesInFlight; ++i)
		{

		shaderData[i].buffer._descriptor.range = sizeof(ShaderData::Values);
		auto descriptorInfo = { shaderData[i].buffer._descriptor};
		//auto uboWriteSet = initialisers::descriptors::descriptor_write_set(descriptorSets.board[0], 0, vk::DescriptorType::eUniformBuffer, descriptorInfo);
		auto uboWriteSet = vk::WriteDescriptorSet{};
		uboWriteSet
				.setDstSet(descriptorSets[i])
				.setDstBinding(0)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setBufferInfo(descriptorInfo);
		writeDescriptors.push_back(uboWriteSet);
		}

		state.device.updateDescriptorSets(writeDescriptors, nullptr);


		auto setLayouts = { descriptorSetLayout };
		vk::PushConstantRange pushConstantRange = vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0,sizeof(glm::mat4));
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo
			.setPushConstantRanges(pushConstantRange)
			.setSetLayouts(setLayouts);

		pipelineLayout = state.device.createPipelineLayout(pipelineLayoutInfo);
		ENGINE_ASSERT(pipelineLayout != vk::PipelineLayout{}, "FAiled to create pipeline layout");
	}

	void egakeru::createGraphicsPipeline()
	{
		PipelineDescription description{};
		description.pipelineLayout = pipelineLayout;
		description.renderPass = renderPass->get();
		description.swapchainExtent = swapchainExtent;

		pipeline = Pipeline::create(description);

		//SpriteRenderer::create(pipelineInfo, swapchainExtent);

	}

	vk::ShaderModule egakeru::createShaderModule(const std::vector<char>& code)
	{
		
		PROFILE_FUNCTION()
		vk::ShaderModuleCreateInfo createInfo{};

		createInfo.setCodeSize(code.size());
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		auto module = state.device.createShaderModule(createInfo);
		ENGINE_ASSERT(module != vk::ShaderModule{}, "Failed to create shader module");

		return module;
	}

	void egakeru::createFramebuffers()
	{
		PROFILE_FUNCTION()
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
				.setRenderPass(renderPass->get())
				.setAttachments(attachments)
				.setWidth(swapchainExtent.width)
				.setHeight(swapchainExtent.height)
				.setLayers(1);

			auto framebuffer = state.device.createFramebuffer(framebufferInfo);
			ENGINE_ASSERT(framebuffer != vk::Framebuffer{}, "Failed to create framebuffer");
			swapChainFramebuffers.emplace_back(framebuffer);
		}
	}

	void egakeru::createCommandPool()
	{
		PROFILE_FUNCTION()
		auto queuFamilyIndices = findQueueFamiles(state.physicalDevice);

		vk::CommandPoolCreateInfo poolInfo{};
		poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		poolInfo.setQueueFamilyIndex(queuFamilyIndices.graphicsFamily.value());

		state.commandPool = state.device.createCommandPool(poolInfo);
		ENGINE_ASSERT(state.commandPool != vk::CommandPool{}, "FAiled to create command pool");
	}

	void egakeru::createDepthResources()
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

	void egakeru::createTextureSampler()
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

	void egakeru::createVertexBuffer()
	{
//		vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

//		BufferProperties properties =
//		{
//			.size = bufferSize,
//			.usage = vk::BufferUsageFlagBits::eTransferSrc,
//			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
//		};
//
//		auto stagingBuffer = Buffer(properties);
//		stagingBuffer.map(vertices.data());
//
//		BufferProperties vertexBufferProperties =
//		{
//			.size = bufferSize,
//			.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
//			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
//		};
//
//		vertexBuffer = Buffer(vertexBufferProperties);
//		vertexBuffer.copy(stagingBuffer);

//		stagingBuffer.destroy();
	}

	void egakeru::createIndexBuffer()
	{
		vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		BufferProperties properties =
		{
			.size = bufferSize,
			.usage = vk::BufferUsageFlagBits::eTransferSrc,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
		};
		auto stagingBuffer = Buffer(properties);
		stagingBuffer.map(indices.data());

		BufferProperties indexBufferProperties =
		{
			.size = bufferSize,
			.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
		};
		indexBuffer = Buffer(indexBufferProperties);
		indexBuffer.copy(stagingBuffer);

		stagingBuffer.destroy();
	}

	void egakeru::createUniformBuffers()
	{
		vk::DeviceSize bufferSize = sizeof(BoardProperties);

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
			boardPropertiesBuffer[i] = Buffer(properties);
			BufferProperties fproperties =
			{
				.size = sizeof(shaderData[i].values),
				.usage = vk::BufferUsageFlagBits::eUniformBuffer,
				.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
			};
			shaderData[i].buffer = Buffer(fproperties);
			shaderData[i].values.model = glm::translate(shaderData[i].values.model, glm::vec3(0, 0, -1));
			shaderData[i].values.model = glm::rotate(shaderData[i].values.model, glm::radians(90.f), glm::vec3(1, 0, 0));
			shaderData[i].values.model = glm::rotate(shaderData[i].values.model, glm::radians(90.f), glm::vec3(0, 0, 1));
		}


	}

	void egakeru::createCommandBuffers()
	{
		_commandBuffers = CommandBuffer(MaxFramesInFlight);
	}

	void egakeru::createSyncObjects()
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

	std::shared_ptr<egkr::Sprite> egakeru::createSprite(const Texture2D& image)
	{
		return SpriteRenderer::createSprite({ 200, 200 }, image);
	}

	void egakeru::updateUniformBuffer(const BoardProperties& boardProperties, uint32_t currentImage)
	{
		boardPropertiesBuffer[currentImage].map(&boardProperties);

		shaderData[currentImage].buffer.map(&shaderData[currentImage].values);
	}

	void egakeru::drawFrame(const BoardProperties& boardProperties, Model::SharedPtr model)
	{
		PROFILE_FUNCTION()
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
			.setRenderPass(renderPass->get())
			.setFramebuffer(swapChainFramebuffers[imageIndex])
			.setRenderArea(vk::Rect2D({ 0, 0 }, swapchainExtent));

		vk::ClearValue clearColour;
		clearColour.color.setFloat32({ clearColor.x, clearColor.y, clearColor.z, clearColor.w });
		vk::ClearValue clearDepth;
		clearDepth.depthStencil = { {1.f, 0} };

		auto clearValues = { clearColour, clearDepth };
		renderPassInfo.setClearValues(clearValues);

		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		//vk::DeviceSize offsets = { 0 };
		//commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, offsets);
		//commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);


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


		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSets[currentFrame], nullptr);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->get_pipeline());
		//commandBuffer.drawIndexed(6, 1, 0, 0, 0);

		//SpriteRenderer::render(commandBuffer, currentFrame);
		model->draw(commandBuffer, pipelineLayout);
		commandBuffer.endRenderPass();
			});
		{
			Gui::Window wind(_gui.get(), "Lighting");

			wind.var("Light Pos", shaderData[0].values.lightPos, -100.f, 100.f, 0.1f);
			shaderData[1].values.lightPos = shaderData[0].values.lightPos;
		}
		_gui->render(swapchainExtent, currentFrame, imageIndex);

		vk::SubmitInfo submitInfo{};

		auto waitSemaphore = { imageAvailableSemaphores[currentFrame] };

		vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		submitInfo.setWaitSemaphores(waitSemaphore);
		submitInfo.setWaitDstStageMask(waitStages);

		auto commandBuffers = { commandBuffer, _gui->getCommandBuffer(currentFrame)};
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

	void egakeru::recreateSwapChain()
	{
		PROFILE_FUNCTION()
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

	void egakeru::cleanupSwapchain()
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

	void egakeru::cleanup()
	{
		PROFILE_FUNCTION()
		SpriteRenderer::destroy();

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
			boardPropertiesBuffer[i].destroy();
			shaderData[i].buffer.destroy();
		}

		state.device.destroyDescriptorSetLayout(descriptorSetLayout);
		state.device.destroyPipelineLayout(pipelineLayout);
		state.device.destroyShaderModule(fragShaderModule);
		state.device.destroyShaderModule(vertShaderModule);

		state.instance.destroySurfaceKHR(surface);
		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(state.instance, _debugMessenger, nullptr);
		}
		renderPass.reset();
		pipeline.reset();

		state.device.destroy();
		state.instance.destroy();
	}
}