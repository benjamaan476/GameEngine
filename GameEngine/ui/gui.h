#pragma once

#include "../EngineCore.h"
#include "../Image.h"
#include "../CommandBuffer.h"

#include <imgui.h>
#include <filesystem>


class GuiImpl;

// Helper to check if a class is a vector
template<typename T, typename = void>
struct is_vector : std::false_type {};

template<typename T>
struct is_vector<T, std::void_t<typename T::value_type>> : std::true_type {};


class Gui
{
public:
	using UniquePtr = std::unique_ptr<Gui>;

	struct DropdownValue
	{
		uint32_t index{};
		std::string label{};
	};

	using DropdownList = std::vector<DropdownValue>;

	struct RadioButton
	{
		uint32_t id{};
		std::string label{};
		bool sameLine{};
	};

	using RadioButtonGroup = std::vector<RadioButton>;

	enum class TextFlags
	{
		Empty = 0x0,
		FitWindow = 0x1
	};

	enum class WindowFlags
	{
		Empty = 0x0,
		ShowTitleBar = 0x1,
		AllowMove = 0x2,
		SetFocus = 0x4,
		CloseButton = 0x8,
		NoResize = 0x10,
		AutoResize = 0x20,

		Default = ShowTitleBar | AllowMove | SetFocus | CloseButton
	};

	enum class WidgetFlags
	{
		Empty = 0x0,
		SameLine = 0x1,
		Inactive = 0x2,
	};

	class Group;

	class Widget
	{
	public:
		Group group(const std::string& label, bool beginExpanded = false);

		void indent(float i);
		void separator(uint32_t count = 1);
		void dummy(const std::string& label, const float2& size, bool sameLine = false);
		void rect(const float2& size, const float4& colour = { 1.f, 1.f, 1.f, 1.f }, bool filled = false, bool sameLine = false);
		bool dropdown(const std::string& label, const DropdownList& values, uint32_t& var, bool sameLine = false);
		bool button(const std::string& label, bool sameLine = false);
		bool radioButtons(const RadioButtonGroup& buttons, uint32_t& activeId);
		bool direction(const std::string& label, float3& direction);
		template<typename T>
		bool checkbox(const std::string& label, T& var, bool sameLine = false);
		bool dragDropSource(const std::string& label, const std::string& dataLabel, const std::string& payload);
		bool dragDropDestination(const std::string& dataLabel, std::string& payload);
		void text(const std::string& text, bool sameLine = false);
		void textWrapped(const std::string& text);
		bool textbox(const std::string& label, std::string& text, TextFlags flags = TextFlags::Empty);
		bool textboxMultiline(const std::string& label, const std::vector<std::string>& textLabels, std::vector<std::string>& textEntries);
		void tooltip(const std::string& text, bool sameLine = false);

		bool rgbColour(const std::string& label, float3& var, bool sameLine = false);
		bool rgbaColour(const std::string& label, float4& var, bool sameLine = false);

		void image(const std::string& label, const Image& image, float2 size = float2{}, bool maintainRatio = true, bool sameLine = false);
		void imageButton(const std::string& label, const Image& image, float2 size, bool maintainRatio = true, bool sameLine = false);

		template<typename T, std::enable_if_t<!is_vector<T>::value, bool> = true>
		bool var(const std::string& label, T& var, T minValue = std::numeric_limits<T>::lowest(), T maxValue = std::numeric_limits<T>::max(), T step = std::is_floating_point_v<T> ? 0.001f : 1.f, bool sameLine = false);

		template<typename T, std::enable_if_t<!is_vector<T>::value, bool> = true>
		bool slider(const std::string& label, T& var, T minValue = std::numeric_limits<T>::lowest() / 2, T maxValue = std::numeric_limits<T>::max() / 2, bool sameLine = false);

		template<typename T, std::enable_if_t<is_vector<T>::value, bool> = true>
		bool var(const std::string& label, T& var, T minValue = std::numeric_limits<T>::lowest(), T maxValue = std::numeric_limits<T>::max(), T step = std::is_floating_point_v<T> ? 0.001f : 1.f, bool sameLine = false);

		template<typename T, std::enable_if_t<is_vector<T>::value, bool> = true>
		bool slider(const std::string& label, T& var, T minValue = std::numeric_limits<T>::lowest() / 2, T maxValue = std::numeric_limits<T>::max() / 2, bool sameLine = false);


		template<typename MatrixType>
		bool matrix(const std::string& label, MatrixType& var, float minValue = -FLT_MAX, float maxValue = FLT_MAX, bool sameLine = false);


		Gui* gui() const { return _gui; }

	protected:
		Widget() = default;
		Gui* _gui{};
	};

	class Group : public Widget
	{
	public:
		Group() = default;
		Group(Gui* gui, const std::string& label, bool beginExpanded = false);
		Group(const Widget& widget, const std::string label, bool beginExpanded = false);

		bool isOpen() const;
		operator bool() const { return isOpen(); }

		~Group();

		void release();
	};

	class Window : public Widget
	{
	public:
		Window(Gui* gui, const std::string& name, uint2 size = { 0, 0 }, uint2 position = { 0, 0 }, WindowFlags flags = WindowFlags::Default);
		Window(Gui* gui, const std::string& name, bool& open, uint2 size = { 0, 0 }, uint2 position = { 0, 0 }, WindowFlags flags = WindowFlags::Default);
		Window(const Widget& widget, const std::string& name, uint2 size = { 0, 0 }, uint2 position = { 0, 0 }, WindowFlags flags = WindowFlags::Default);
		Window(const Widget& widget, const std::string& name, bool& open, uint2 size = { 0, 0 }, uint2 position = { 0, 0 }, WindowFlags flags = WindowFlags::Default);

		~Window();

		void release();
		void columns(uint32_t numColumns);
		void nextColumn();
		void windowPosition(uint32_t x, uint32_t y);
		void windowSize(uint32_t x, uint32_t y);
	};
	static UniquePtr create(uint32_t width, uint32_t height, float scaleFactor = 1.f);
	~Gui();

	static float4 pickUniqueColour(const std::string& key);

	void addFont(const std::string& name, const std::filesystem::path& path);

	void setActiveFont(const std::string& font);

	ImFont* getFont(std::string f = "");

	void begin();
	void render(CommandBuffer buffer, vk::RenderPass renderPass, vk::Framebuffer framebuffer, vk::Extent2D extent, uint32_t currentFrame, uint32_t imageIndex);
	static void setGlobalScaling(float scale);
	void onWindowResize(uint32_t width, uint32_t height);

private:
	Gui() = default;
	GuiImpl* _wrapper = nullptr;

};

ENUM_CLASS_OPERATORS(Gui::WindowFlags)
ENUM_CLASS_OPERATORS(Gui::TextFlags)
