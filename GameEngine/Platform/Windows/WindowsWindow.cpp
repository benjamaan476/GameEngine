#include "WindowsWindow.h"

Window::SharedPtr WindowsWindow::create(const WindowProperties& properties)
{
	return std::shared_ptr<WindowsWindow>(new WindowsWindow(properties));
}

vk::SurfaceKHR WindowsWindow::createSurface(const vk::Instance& instance)
{
	vk::SurfaceKHR surface;
	glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface));
	return surface;
}

WindowsWindow::~WindowsWindow()
{
	Shutdown();
}

void WindowsWindow::OnUpdate() const
{
	glfwPollEvents();

}

bool WindowsWindow::isRunning() const
{
	return glfwWindowShouldClose(window);
}

std::vector<const char*> WindowsWindow::GetRequiredExtensions() const
{
	uint32_t extensionCount = 0;
	auto extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> extension(extensions, extensions + extensionCount);
	return  extension;
}

void WindowsWindow::SetVSync(bool enable)
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

bool WindowsWindow::IsVSync() const
{
	return data.VSync;
}

std::pair<uint32_t, uint32_t> WindowsWindow::getFramebufferSize() const
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	return {(uint32_t)width, (uint32_t)height};
}

void WindowsWindow::waitEvents()
{
	glfwWaitEvents();
}

WindowsWindow::WindowsWindow(const WindowProperties& properties)
{
	data = WindowData
	{
		.title = properties.title,
		.width = properties.width,
		.height = properties.height,
	};

	if (!isGLFWInitialised)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		isGLFWInitialised = true;
	}

	window = glfwCreateWindow(data.width, data.height, data.title.data(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &data);

}


void WindowsWindow::Shutdown()
{
	if (isGLFWInitialised)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}
