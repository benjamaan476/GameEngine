#include "LinuxWindow.h"
#include "../../EngineCore.h"

EngineWindow::SharedPtr LinuxWindow::create(const WindowProperties& properties)
{
	return std::shared_ptr<LinuxWindow>(new LinuxWindow(properties));
}

vk::SurfaceKHR LinuxWindow::createSurface(const vk::Instance& instance, const vk::AllocationCallbacks allocator)
{
	vk::SurfaceKHR surface;
	glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface));
	return surface;
}

LinuxWindow::~LinuxWindow()
{
	Shutdown();
}

void LinuxWindow::OnUpdate() const
{
	glfwPollEvents();

}

bool LinuxWindow::isRunning() const
{
	return glfwWindowShouldClose(window);
}

std::vector<const char*> LinuxWindow::GetRequiredExtensions() const
{
	uint32_t extensionCount = 0;
	auto extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> extension(extensions, extensions + extensionCount);
	return  extension;
}

void LinuxWindow::SetVSync(bool enable)
{
	if (enable)
	{
		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(0);
	}

	data.VSync = enable;
}

bool LinuxWindow::IsVSync() const
{
	return data.VSync;
}

std::pair<uint32_t, uint32_t> LinuxWindow::getFramebufferSize() const
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	return { (uint32_t)width, (uint32_t)height };
}

void LinuxWindow::waitEvents()
{
	glfwWaitEvents();
}

void error_callback(int error, const char* msg) {
	std::string s;
	s = " [" + std::to_string(error) + "] " + msg + '\n';
	ENGINE_ASSERT(false, s)
}

LinuxWindow::LinuxWindow(const WindowProperties& properties)
{
	data = WindowData
	{
		.title = properties.title,
		.width = properties.width,
		.height = properties.height,
	};

	if (!isGLFWInitialised)
	{
		auto success = glfwInit();

		ENGINE_ASSERT(success != GLFW_TRUE, "")
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwSetErrorCallback(error_callback);
		isGLFWInitialised = true;
	}

	window = glfwCreateWindow(data.width, data.height, data.title.data(), glfwGetPrimaryMonitor(), NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &data);

}


void LinuxWindow::Shutdown()
{
	if (isGLFWInitialised)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}
