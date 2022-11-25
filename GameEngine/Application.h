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
	static std::vector<std::filesystem::path> getTextureDirectories() { return gTextureDirectories; }
	static std::vector<std::filesystem::path> getShaderDirectories() { return gShaderDirectories; }
	
	auto& window() const { return _window; }
private:

	const static inline std::filesystem::path& getExecutableDirectory()
	{
		static std::filesystem::path directory;
		if (directory.empty())
		{
			directory = std::filesystem::current_path();
		}

		return directory;
	}

	static inline std::vector<std::filesystem::path> getInitialShaderDirectories()
	{
		std::filesystem::path projectDir(_PROJECT_DIR_);
		projectDir = projectDir.make_preferred();
		std::vector<std::filesystem::path> developmentDirectories =
		{
			// First we search in source folders.
			projectDir,
			projectDir / "shaders",
			// Then we search in deployment folder (necessary to pickup NVAPI and other third-party shaders).
			getExecutableDirectory() / "shaders",
		};

		std::vector<std::filesystem::path> deploymentDirectories =
		{
			getExecutableDirectory() / "shaders",
		};

#ifdef NDEBUG
		return deploymentDirectories;
#else
		return developmentDirectories;
#endif
	}

	static inline std::vector<std::filesystem::path> getInitialTextureDirectories()
	{
		std::filesystem::path projectDir(_PROJECT_DIR_);
		projectDir = projectDir.make_preferred();
		std::vector<std::filesystem::path> developmentDirectories =
		{
			// First we search in source folders.
			projectDir,
			projectDir / "textures",
			// Then we search in deployment folder (necessary to pickup NVAPI and other third-party shaders).
			getExecutableDirectory() / "textures",
		};

		std::vector<std::filesystem::path> deploymentDirectories =
		{
			getExecutableDirectory() / "textures",
		};

#ifdef NDEBUG
		return deploymentDirectories;
#else
		return developmentDirectories;
#endif
	}

	const static inline std::vector<std::filesystem::path> gShaderDirectories = getInitialShaderDirectories();
	const static inline std::vector<std::filesystem::path> gTextureDirectories = getInitialTextureDirectories();


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

	void dragDropCallback(int pathCount, const char** paths);

	//Image textureImage;
	//vk::Sampler textureSampler;

	bool dropped = false;
	std::string _droppedPayload;


	Texture2D image;
};

