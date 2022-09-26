#include "Window.h"
#include "Windows/WindowsWindow.h"

Window::SharedPtr Window::create(const WindowProperties& properties)
{
    return WindowsWindow::create(properties);
}
