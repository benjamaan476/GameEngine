#include "WindowsWindow.h"

Window::SharedPtr WindowsWindow::create(const WindowProperties& properties)
{
	return std::shared_ptr<WindowsWindow>(new WindowsWindow(properties));
}

WindowsWindow::~WindowsWindow()
{
	Shutdown();
}

void WindowsWindow::OnUpdate()
{
}

void WindowsWindow::SetVSync(bool enable)
{
	//glfwWindowHint(GLFW_)
}

bool WindowsWindow::IsVSync() const
{
	return false;
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
}

void WindowsWindow::Shutdown()
{
	if (isGLFWInitialised)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

}
