#include "gui.h"

#include <imgui.h>

class GuiImpl
{
public:
	GuiImpl() = default;

private:
	friend class Gui;
	void init(Gui* gui, float scaleFactor);
	void compileFonts();

	bool addCheckboxes(const std::string& label, std::span<bool> data, bool sameLine = false);

	struct ComboData
	{
		uint32_t lastValue{};
		int32_t currentItem{};
	};

	std::unordered_map<std::string, ComboData> _dropDownValues;

	std::vector<Image> _images;
	float _scaleFactor = 1.f;

	bool pushWindow(const std::string& name, bool& open, uint2 size = { 0, 0 }, uint2 position = { 0, 0 }, Gui::WindowFlags flags = Gui::WindowFlags::Default);
	void popWindow();
	void setCurrentWindowSize(uint32_t width, uint32_t height);
	void setCurrentWindowPosition(uint32_t x, uint32_t y);
	void beginColumns(uint32_t numColumns);
	void nextColumn();

	bool beginGroup(const std::string& label, bool beginExpanded = false);
	bool endGroup();

	void indent(float i);
	void addSeparator(uint32_t count = 1);
	void addDummyItem(const std::string& label, const float2& size, bool sameLine = false);
	void addRect(const float2& size, const float4& color = float4(1.0f, 1.0f, 1.0f, 1.0f), bool filled = false, bool sameLine = false);
	bool addDropdown(const std::string& label, const Gui::DropdownList& values, uint32_t& var, bool sameLine = false);
	bool addButton(const std::string& label, bool sameLine = false);
	bool addRadioButtons(const Gui::RadioButtonGroup& buttons, uint32_t& activeID);
	bool addDirectionWidget(const std::string& label, float3& direction);
	bool addCheckbox(const std::string& label, bool& var, bool sameLine = false);
	bool addCheckbox(const std::string& label, int& var, bool sameLine = false);
	template<typename T>
	bool addBoolVecVar(const std::string& label, T& var, bool sameLine = false);
	bool addDragDropSource(const std::string& label, const char dataLabel[], const std::string& payloadString);
	bool addDragDropDest(const std::string& dataLabel, std::string& payloadString);

	void addText(const std::string& text, bool sameLine = false);
	void addTextWrapped(const std::string& text);
	bool addTextbox(const std::string& label, std::string& text, uint32_t lineCount = 1, Gui::TextFlags flags = Gui::TextFlags::Empty);
	bool addTextbox(const std::string& label, char buf[], size_t bufSize, uint32_t lineCount = 1, Gui::TextFlags flags = Gui::TextFlags::Empty);
	bool addMultiTextbox(const std::string& label, const std::vector<std::string>& textLabels, std::vector<std::string>& textEntries);
	void addTooltip(const std::string& tip, bool sameLine = true);

	bool addRgbColor(const std::string& label, float3& var, bool sameLine = false);
	bool addRgbaColor(const std::string& label, float4& var, bool sameLine = false);

	void addImage(const std::string& label, const Image& image, float2 size = float2{}, bool maintainRation = true, bool sameLine = false);
	void addImageButton(const std::string& label, const Image& image, float2 size, bool maintainRation = true, bool sameLine = false);

	template<typename T>
	bool addScalarVar(const std::string& label, T& var, T minVal = std::numeric_limits<T>::lowest(), T maxVal = std::numeric_limits<T>::max(), float step = 1.0f, bool sameLine = false, const char* displayFormat = nullptr);
	template<typename T>
	bool addScalarSlider(const std::string& label, T& var, T minVal = std::numeric_limits<T>::lowest(), T maxVal = std::numeric_limits<T>::max(), bool sameLine = false, const char* displayFormat = nullptr);

	template<typename T>
	bool addVecVar(const std::string& label, T& var, typename T::value_type minVal = std::numeric_limits<typename T::value_type>::lowest(), typename T::value_type maxVal = std::numeric_limits<typename T::value_type>::max(), float step = 1.0f, bool sameLine = false, const char* displayFormat = nullptr);
	template<typename T>
	bool addVecSlider(const std::string& label, T& var, typename T::value_type minVal = std::numeric_limits<typename T::value_type>::lowest(), typename T::value_type maxVal = std::numeric_limits<typename T::value_type>::max(), bool sameLine = false, const char* displayFormat = nullptr);

	template<int R, int C, typename T>
	void addMatrixVar(const std::string& label, glm::mat<C, R, T>& var, float minValue = -FLT_MIN, float maxValue = FLT_MAX, bool sameLine = false);
};

