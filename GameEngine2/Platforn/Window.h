#pragma once

#include "../EngineCore.h"

struct WindowProperties
{
	std::string title;
	uint32_t width;
	uint32_t height;
};

class Window
{
public:

	using SharedPtr = std::shared_ptr<Window>;
	using EventCallback = std::function<void(void)>;

	virtual ~Window() {}

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;

	virtual void SetEventCallback(const EventCallback& callback) = 0;
	virtual void SetVSync(bool enable) = 0;
	virtual bool IsVSync() const = 0;

	virtual void OnUpdate() = 0;
};

