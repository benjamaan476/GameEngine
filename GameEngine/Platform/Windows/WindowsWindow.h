#pragma once

#include <GLFW/glfw3.h>

#include "../Window.h"
#include "../../EngineCore.h"

class WindowsWindow : public Window
{
public:
	Window::SharedPtr create(const WindowProperties& properties);
	virtual ~WindowsWindow() override;

	void OnUpdate() override;

	inline uint32_t GetWidth() const override { return data.width; }
	inline uint32_t GetHeight() const override { return data.height; }

	void SetEventCallback(const EventCallback& callback)  override { data.callback = callback; }
	void SetVSync(bool enable) override;
	bool IsVSync() const override;
private:
	WindowsWindow(const WindowProperties& properties);

	virtual void Shutdown();

private:

	static inline bool isGLFWInitialised = false;

	GLFWwindow* window;
	struct WindowData
	{
		std::string title;
		uint32_t width;
		uint32_t height;
		EventCallback callback;
	} data;
};

