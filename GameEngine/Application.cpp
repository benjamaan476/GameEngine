#include "Application.h"
#include "Log/Log.h"

Application::Application(std::string_view name, uint32_t width, uint32_t height)
{
	Log::Init();
	LOG_INFO("Initialized");
	_instance = this;
	initWindow(name, width, height);

	Renderer::create();
	initGui();
}

void Application::run()
{
	mainLoop();
	cleanup();
}

void Application::initWindow(std::string_view name, uint32_t width, uint32_t height)
{
	WindowProperties windowProperties{ name.data(), width, height};

	auto callback = std::bind(&Application::dragDropCallback, this, std::placeholders::_1, std::placeholders::_2);

	auto cbk = [](int pathCount, const char** paths)
	{
		if (pathCount)
		{
			std::string str(paths[0]);
			LOG_TRACE("File Dropped: {0}", str);
		}
	};

	_window = Window::create(windowProperties, callback);
}

void Application::initGui()
{
	_gui = Gui::create();
}

void Application::mainLoop()
{
	bool show_demo_window = false;
	float4 clear_color{ 0.45f, 0.55f, 0.60f, 1.00f };
	int counter = 0;

	while (!_window->isRunning())
	{
		_window->OnUpdate();
		
		{
			_gui->begin();

			_gui->demo(show_demo_window);
			
			boardProperties.onRender(_gui.get());

			Gui::Window wind(_gui.get(), "Hello, World");

			if(wind.button("Show debug window"))
			{
				show_demo_window = !show_demo_window;
			}


			wind.text("This is some useful text.");               // Display some text (you can use a format strings too)
			wind.checkbox("Demo Window", show_demo_window);      // Edit bools storing our window open/close state

			//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//wind.rgbaColour("clear color", clearColor); // Edit 3 floats representing a color

			if (wind.button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;

			auto count = std::format("counter = {}", counter);
			wind.text(count, true);

			auto& textureImage = Renderer::getTextureImage();
			auto& textureSampler = Renderer::getTextureSampler();

			std::string payload;
			wind.dragDropDestination("Image drop", payload);

			wind.image("Image", textureImage, textureSampler);

			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		Renderer::drawFrame(boardProperties);
	}

	state.device.waitIdle();
}

void Application::destroyUi()
{
	Gui::release();
}

void Application::cleanup()
{
	destroyUi();
	Renderer::cleanup();

}

void Application::dragDropCallback(int pathCount, const char** paths) const
{
	if(pathCount)
	{
		std::string str(paths[0]);
		LOG_TRACE("File Dropped: {0}", str);
	}
}
