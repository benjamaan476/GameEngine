#pragma once
#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


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


	void createInstance();
	bool checkValidationLayerSupport() const;
private:
	std::string name;
	uint32_t width;
	uint32_t height;
	GLFWwindow* window = nullptr;

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

};

