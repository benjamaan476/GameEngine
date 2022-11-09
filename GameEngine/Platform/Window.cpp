#include "Window.h"

#include "../EngineCore.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/WindowsWindow.h"
#elif defined(PLATFORM_LINUX)
#include "Windows/LinuxWindow.h"
#endif
EngineWindow::SharedPtr EngineWindow::create(const WindowProperties& properties)
{
#if PLATFORM_WINDOWS
    return WindowsWindow::create(properties);
#elif defined(PLATFORM_LINUX)
    return LinuxWindow::create(properties);
#endif
}
