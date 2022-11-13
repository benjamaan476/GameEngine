#pragma once
#include "EngineCore.h"

#include "Platform/Window.h"
#include <optional>

#include "ui/gui.h"
#include "renderer/Renderer.h"

#include "BoardProperties.h"

class Application
{
public:
	Application(std::string_view name, uint32_t width, uint32_t height);
	void run();

	static Application& get() { return *_instance; }
	auto& window() const { return _window; }
private:

	void initWindow(std::string_view name, uint32_t width, uint32_t height);
	void initGui();
	void mainLoop();
	void destroyUi();
	void cleanup();

	static inline Application* _instance = nullptr;
private:

	Window::SharedPtr _window;
	Gui::SharedPtr _gui;
	BoardProperties boardProperties;
};

