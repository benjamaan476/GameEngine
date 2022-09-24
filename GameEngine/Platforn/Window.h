#pragma once

#include <vulkan/vulkan.hpp>

#include <string>
#include <memory>
#include <functional>
#include <tuple>

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

	static SharedPtr create(const WindowProperties& properties);
	virtual vk::SurfaceKHR createSurface(const vk::Instance& instance) = 0;
	virtual ~Window() = default;

	virtual void OnUpdate() const = 0;
	virtual bool isRunning() const = 0;
	
	virtual std::vector<const char*> GetRequiredExtensions() const = 0;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;

	virtual void SetEventCallback(const EventCallback& callback) = 0;
	virtual void SetVSync(bool enable) = 0;
	virtual bool IsVSync() const = 0;


};

