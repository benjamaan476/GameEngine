#include "gui.h"

#include <imgui.h>
#include <span>

#include <glm/gtc/type_ptr.hpp>

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
	uint32_t _groupStackSize{};

	bool pushWindow(const std::string& name, bool& open, uint2 size = { 0, 0 }, uint2 position = { 0, 0 }, Gui::WindowFlags flags = Gui::WindowFlags::Default);
	void popWindow();
	void setCurrentWindowSize(uint32_t width, uint32_t height);
	void setCurrentWindowPosition(uint32_t x, uint32_t y);
	void beginColumns(uint32_t numColumns);
	void nextColumn();

	bool beginGroup(const std::string& label, bool beginExpanded = false);
	void endGroup();

	void indent(float i);
	void addSeparator(uint32_t count = 1);
	void addDummyItem(const std::string& label, const float2& size, bool sameLine = false);
	void addRect(const float2& size, const float4& colour = float4(1.0f, 1.0f, 1.0f, 1.0f), bool filled = false, bool sameLine = false);
	bool addDropdown(const std::string& label, const Gui::DropdownList& values, uint32_t& var, bool sameLine = false);
	bool addButton(const std::string& label, bool sameLine = false);
	bool addRadioButtons(const Gui::RadioButtonGroup& buttons, uint32_t& activeID);
	bool addDirection(const std::string& label, float3& direction);
	bool addCheckbox(const std::string& label, bool& var, bool sameLine = false);
	bool addCheckbox(const std::string& label, int& var, bool sameLine = false);
	
	template<typename T>
	bool addBoolVecVar(const std::string& label, T& var, bool sameLine = false);
	
	bool addDragDropSource(const std::string& label, const std::string& dataLabel, const std::string& payloadString);
	bool addDragDropDest(const std::string& dataLabel, std::string& payloadString);

	void addText(const std::string& text, bool sameLine = false);
	void addTextWrapped(const std::string& text);
	bool addTextbox(const std::string& label, std::string& text, uint32_t lineCount = 1, Gui::TextFlags flags = Gui::TextFlags::Empty);
	bool addMultiTextbox(const std::string& label, const std::vector<std::string>& textLabels, std::vector<std::string>& textEntries);
	void addTooltip(const std::string& tip, bool sameLine = true);

	bool addRgbColor(const std::string& label, float3& var, bool sameLine = false);
	bool addRgbaColor(const std::string& label, float4& var, bool sameLine = false);

	void addImage(const std::string& label, const Image& image, float2 size = float2{}, bool maintainRatio = true, bool sameLine = false);
	bool addImageButton(const std::string& label, const Image& image, float2 size, bool maintainRatio = true, bool sameLine = false);

	template<typename T>
	bool addScalarVar(const std::string& label, T& var, T minVal = std::numeric_limits<T>::lowest(), T maxVal = std::numeric_limits<T>::max(), float step = 1.0f, bool sameLine = false, const char* displayFormat = nullptr);
	template<typename T>
	bool addScalarSlider(const std::string& label, T& var, T minVal = std::numeric_limits<T>::lowest(), T maxVal = std::numeric_limits<T>::max(), bool sameLine = false, const char* displayFormat = nullptr);

	template<typename T>
	bool addVecVar(const std::string& label, T& var, typename T::value_type minVal = std::numeric_limits<typename T::value_type>::lowest(), typename T::value_type maxVal = std::numeric_limits<typename T::value_type>::max(), float step = 1.0f, bool sameLine = false, const char* displayFormat = nullptr);
	template<typename T>
	bool addVecSlider(const std::string& label, T& var, typename T::value_type minVal = std::numeric_limits<typename T::value_type>::lowest(), typename T::value_type maxVal = std::numeric_limits<typename T::value_type>::max(), bool sameLine = false, const char* displayFormat = nullptr);

	template<int R, int C, typename T>
	void addMatrixVar(const std::string& label, glm::mat<C, R, T>& var, float minValue = std::numeric_limits<float>::lowest(), float maxValue = std::numeric_limits<float>::max(), bool sameLine = false);
};

void GuiImpl::init(Gui* gui, float scaleFactor)
{
	_scaleFactor = scaleFactor;
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void GuiImpl::compileFonts()
{

}

bool GuiImpl::addCheckboxes(const std::string& label, std::span<bool> data, bool sameLine)
{
	auto modified = false;
	std::string labelString{ std::string("##") + label + '0' };
	
	for (auto i = 0; i < data.size() - 1; i++)
	{
		labelString[labelString.size() - 1] = '0' + static_cast<int32_t>(i);
		modified |= addCheckbox(labelString, data[i], !i ? sameLine : true);
	}

	addCheckbox(label, data[data.size() - 1], true);

	return modified;
}

bool GuiImpl::pushWindow(const std::string& label, bool& open, uint2 size, uint2 position, Gui::WindowFlags flags)
{
	bool allowClose = isSet(flags, Gui::WindowFlags::CloseButton);
	if (allowClose)
	{
		if (!isSet(flags, Gui::WindowFlags::ShowTitleBar))
		{
			LOG_WARN("{}", "Asking for a close button on a window without a title bar");
		}
	}

	float2 posFloat{ position};
	posFloat *= _scaleFactor;

	ImGui::SetNextWindowSize({ (float)size.x, (float)size.y }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos({ posFloat.x, posFloat.y }, ImGuiCond_FirstUseEver);

	auto imguiFlags = 0;

	if (!isSet(flags, Gui::WindowFlags::ShowTitleBar))
	{
		imguiFlags |= ImGuiWindowFlags_NoTitleBar;
	}
	if (!isSet(flags, Gui::WindowFlags::AllowMove))
	{
		imguiFlags |= ImGuiWindowFlags_NoMove;
	}
	if (!isSet(flags, Gui::WindowFlags::SetFocus))
	{
		imguiFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
	}
	if (isSet(flags, Gui::WindowFlags::NoResize))
	{
		imguiFlags |= ImGuiWindowFlags_NoResize;
	}
	if (!isSet(flags, Gui::WindowFlags::AutoResize))
	{
		imguiFlags |= ImGuiWindowFlags_AlwaysAutoResize;
	}

	ImGui::Begin(label.c_str(), allowClose ? &open : nullptr, imguiFlags);

	if (!open)
	{
		ImGui::End();
	}

	return open;
}

void GuiImpl::popWindow()
{
	ImGui::End();
}

void GuiImpl::setCurrentWindowPosition(uint32_t x, uint32_t y)
{
	ImGui::SetWindowPos({(float)x, (float)y});
}

void GuiImpl::setCurrentWindowSize(uint32_t width, uint32_t height)
{
	ImGui::SetWindowPos({(float)width, (float)height});
}

void GuiImpl::beginColumns(uint32_t numColumns)
{
	ImGui::Columns(numColumns);
}

void GuiImpl::nextColumn()
{
	ImGui::NextColumn();
}

bool GuiImpl::beginGroup(const std::string& name, bool beginExpanded)
{
	auto flags = beginExpanded ? ImGuiTreeNodeFlags_DefaultOpen : 0;
	auto visible = _groupStackSize ? ImGui::TreeNodeEx(name.c_str(), flags) : ImGui::CollapsingHeader(name.c_str(), flags);
	if (visible)
	{
		_groupStackSize++;
	}

	return visible;
}

void GuiImpl::endGroup()
{
	ENGINE_ASSERT(_groupStackSize >= 1, "Attempting to end a group that wasn't begun");
	_groupStackSize--;
	if (_groupStackSize)
	{
		ImGui::TreePop();
	}
}

void GuiImpl::indent(float i)
{
	ImGui::Indent(i);
}

void GuiImpl::addSeparator(uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		ImGui::Separator();
	}
}

void GuiImpl::addDummyItem(const std::string& label, const float2& size, bool sameLine)
{
	if (sameLine)
	{
		ImGui::SameLine();
	}

	ImGui::PushID(label.c_str());
	ImGui::Dummy({ size.x, size.y });
	ImGui::PopID();
}

void GuiImpl::addRect(const float2& size, const float4& colour, bool filled, bool sameLine)
{
	if (sameLine)
	{
		ImGui::SameLine();
	}

	const auto& cursorPosition = ImGui::GetCursorScreenPos();

	ImVec2 bottomLeft{ cursorPosition.x + size.x, cursorPosition.y + size.y };
	ImVec4 rectColour{ colour.r, colour.g, colour.b, colour.a };

	if (filled)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(cursorPosition, bottomLeft, ImGui::ColorConvertFloat4ToU32(rectColour));
	}
	else
	{
		ImGui::GetWindowDrawList()->AddRect(cursorPosition, bottomLeft, ImGui::ColorConvertFloat4ToU32(rectColour));
	}
}

bool GuiImpl::addDropdown(const std::string& label, const Gui::DropdownList& values, uint32_t& var, bool sameLine)
{
	if (values.empty())
	{
		return false;
	}

	if (sameLine)
	{
		ImGui::SameLine();
	}

	const auto& iter = _dropDownValues.find(label);

	auto currentItem = -1;

	if ((iter == _dropDownValues.end()) || (iter->second.lastValue != var))
	{
		for (auto i = 0; i < values.size(); i++)
		{
			if (values[i].index == var)
			{
				currentItem = i;
				_dropDownValues[label].currentItem = i;
				break;
			}
		}
	}
	else
	{
		currentItem = _dropDownValues[label].currentItem;
	}

	std::string comboString;
	for (const auto& v : values)
	{
		comboString += v.label + '\0';
	}
	comboString += '\0';

	auto previousItem = currentItem;

	auto b = ImGui::Combo(label.c_str(), &currentItem, comboString.c_str());
	_dropDownValues[label].currentItem = currentItem;
	_dropDownValues[label].lastValue = values[currentItem].index;
	var = values[currentItem].index;

	return b && previousItem != currentItem;
}

bool GuiImpl::addButton(const std::string& label, bool sameLine)
{
	if (sameLine)
	{
		ImGui::SameLine();
	}
	return ImGui::Button(label.c_str());
}

bool GuiImpl::addRadioButtons(const Gui::RadioButtonGroup& buttons, uint32_t& activeID)
{
	auto oldValue = activeID;

	for (const auto& button : buttons)
	{
		if (button.sameLine)
		{
			ImGui::SameLine();
		}
		ImGui::RadioButton(button.label.c_str(), (int*)&activeID, button.id);
	}

	return oldValue != activeID;
}

bool GuiImpl::addDirection(const std::string& label, float3& direction)
{
	auto dir = glm::normalize(direction);
	auto b = addVecVar(label, dir, -1.f, 1.f, 0.001f, false, "%.3f");

	if (b)
	{
		direction = glm::normalize(dir);
	}

	return b;
}

bool GuiImpl::addCheckbox(const std::string& label, bool& var, bool sameLine)
{
	if (sameLine)
	{
		ImGui::SameLine();
	}
	return ImGui::Checkbox(label.c_str(), &var);
}

bool GuiImpl::addCheckbox(const std::string& label, int& var, bool sameLine)
{
	auto value = var != 0;
	auto modified = addCheckbox(label, value, sameLine);

	var = value ? 1 : 0;
	return modified;
}

template<typename T>
bool GuiImpl::addBoolVecVar(const std::string& label, T& var, bool sameLine)
{
	return addCheckboxes(label, { glm::value_ptr(var), var.length() }, sameLine);
}

bool GuiImpl::addDragDropSource(const std::string& label, const std::string& dataLabel, const std::string& payloadString)
{
	if (ImGui::IsItemHovered() && (ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)))
	{
		ImGui::SetWindowFocus();
	}

	if (!(ImGui::IsWindowFocused()))
	{
		return false;
	}

	auto b = ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID);

	if (b)
	{
		ImGui::SetDragDropPayload(dataLabel.c_str(), payloadString.c_str(), payloadString.size() * sizeof(payloadString[0]), ImGuiCond_Once);
	}

	return b;
}

bool GuiImpl::addDragDropDest(const std::string& dataLabel, std::string& payloadString)
{
	auto b = false;

	if (ImGui::BeginDragDropTarget())
	{
		auto dragDropPayload = ImGui::AcceptDragDropPayload(dataLabel.c_str());

		b = dragDropPayload && dragDropPayload->IsDataType(dataLabel.c_str()) && (dragDropPayload->Data != nullptr);

		if (b)
		{
			payloadString.resize(dragDropPayload->DataSize);
			std::memcpy(&payloadString.front(), dragDropPayload->Data, dragDropPayload->DataSize);
		}

		ImGui::EndDragDropTarget();
	}

	return b;
}

void GuiImpl::addText(const std::string& text, bool sameLine)
{
	if (sameLine)
	{
		ImGui::SameLine();
	}
	ImGui::TextUnformatted(text.c_str());
}

void GuiImpl::addTextWrapped(const std::string& text)
{
	ImGui::TextWrapped("%s", text.c_str());
}

bool GuiImpl::addTextbox(const std::string& label, std::string& text, uint32_t lineCount, Gui::TextFlags flags)
{
	auto fitWindow = isSet(flags, Gui::TextFlags::FitWindow);

	if (fitWindow)
	{
		ImGui::PushItemWidth(ImGui::GetWindowWidth());
	}

	static const uint32_t maxSize = 2048;
	char buf[maxSize];
	const uint32_t length = std::min(maxSize - 1, (uint32_t)text.length());
	text.copy(buf, length);
	buf[length] = '\0';


	if (lineCount > 1)
	{
		auto flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine;
		return ImGui::InputTextMultiline(label.c_str(), buf, length, { -1.f, ImGui::GetTextLineHeight() * lineCount }, flags);
	}
	else
	{
		return ImGui::InputText(label.c_str(), buf, length, ImGuiInputTextFlags_EnterReturnsTrue);
	}

	if (fitWindow)
	{
		ImGui::PopItemWidth();
	}
}


bool GuiImpl::addMultiTextbox(const std::string& label, const std::vector<std::string>& textLabels, std::vector<std::string>& textEntries)
{
	static uint32_t idOffset = 0;
	auto result = false;

	for (uint32_t i = 0; i < textEntries.size(); i++)
	{
		result |= addTextbox(std::string{ textLabels[i] + "##" + std::to_string(idOffset) }, textEntries[i]);
	}

	return addButton(label) || result;
}

void GuiImpl::addTooltip(const std::string& tip, bool sameLine)
{
	if (sameLine)
	{
		ImGui::SameLine();
	}
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.f);
		ImGui::TextUnformatted(tip.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool GuiImpl::addRgbColor(const std::string& label, float3& var, bool sameLine)
{
	if (sameLine)
	{
		ImGui::SameLine();
	}

	return ImGui::ColorEdit3(label.c_str(), glm::value_ptr(var));
}

bool GuiImpl::addRgbaColor(const std::string& label, float4& var, bool sameLine)
{
	if (sameLine)
	{
		ImGui::SameLine();
	}

	return ImGui::ColorEdit4(label.c_str(), glm::value_ptr(var));
}

void GuiImpl::addImage(const std::string& label, const Image& image, float2 size, bool maintainRatio, bool sameLine)
{
	if (size == float2(0))
	{
		auto windowSize = ImGui::GetWindowSize();
		size = { windowSize.x, windowSize.y };
	}

	ImGui::PushID(label.c_str());
	if (sameLine)
	{
		ImGui::SameLine();
	}
	_images.push_back(image);

	auto aspectRatio = maintainRatio ? (float)image.height / (float)image.width : 1.f;

	ImGui::Image(reinterpret_cast<ImTextureID>(_images.size()), { size.x, maintainRatio ? size.x * aspectRatio : size.y });
}

bool GuiImpl::addImageButton(const std::string& label, const Image& image, float2 size, bool maintainRatio, bool sameLine)
{
	_images.push_back(image);
	if (sameLine)
	{
		ImGui::SameLine();
	}

	auto aspectRatio = maintainRatio ? (float)image.height / (float)image.width : 1.f;
	return ImGui::ImageButton((ImTextureID)_images.size(), { size.x, maintainRatio ? size.x * aspectRatio : size.y });
}


template<typename T>
bool addScalarVarHelper(const std::string& label, T& var, ImGuiDataType_ imguiType, T minValue, T maxValue, float step, bool sameLine, const char* displayFormat)
{
	ImGui::PushItemWidth(200);
	if (sameLine)
	{
		ImGui::SameLine();
	}
	auto b = ImGui::DragScalar(label.c_str(), imguiType, &var, step, &minValue, &maxValue, displayFormat);
	var = glm::clamp(var, minValue, maxValue);
	ImGui::PopItemWidth();

	return b;
}

template<typename T>
struct foobar : std::false_type
{ };

template<typename T>
bool GuiImpl::addScalarVar(const std::string& label, T& var, T minVal, T maxVal, float step, bool sameLine, const char* displayFormat)
{
	inline constexpr bool always_false = false;

	if constexpr (std::is_same<T, int32_t>::value)
	{
		return addScalarVarHelper(label, var, ImGuiDataType_S32, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, uint32_t>::value)
	{
		return addScalarVarHelper(label, var, ImGuiDataType_U32, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, int64_t>::value)
	{
		return addScalarVarHelper(label, var, ImGuiDataType_S64, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, uint64_t>::value)
	{
		return addScalarVarHelper(label, var, ImGuiDataType_U64, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, float_t>::value)
	{
		return addScalarVarHelper(label, var, ImGuiDataType_Float, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, uint64_t>::value)
	{
		return addScalarVarHelper(label, var, ImGuiDataType_U64, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, double_t>::value)
	{
		return addScalarVarHelper(label, var, ImGuiDataType_Double, minVal, maxVal, step, sameLine, displayFormat);
	}
	else
	{
		static_assert(foobar<T>::value, "Unsupported data type");
	}
}

template<typename T>
bool addScalarSliderHelper(const std::string& label, T& var, ImGuiDataType_ imguiType, T minValue, T maxValue, bool sameLine, const char* displayFormat)
{
	ImGui::PushItemWidth(200);
	if (sameLine)
	{
		ImGui::SameLine();
	}
	auto b = ImGui::SliderScalar(label.c_str(), imguiType, &var, &minValue, &maxValue, displayFormat);
	var = glm::clamp(var, minValue, maxValue);
	ImGui::PopItemWidth();

	return b;
}

template<typename T>
bool GuiImpl::addScalarSlider(const std::string& label, T& var, T minVal, T maxVal, bool sameLine, const char* displayFormat)
{
	if constexpr (std::is_same<T, int32_t>::value)
	{
		return addScalarSliderHelper(label, var, ImGuiDataType_S32, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, uint32_t>::value)
	{
		return addScalarSliderHelper(label, var, ImGuiDataType_U32, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, int64_t>::value)
	{
		return addScalarSliderHelper(label, var, ImGuiDataType_S64, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, uint64_t>::value)
	{
		return addScalarSliderHelper(label, var, ImGuiDataType_U64, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, float>::value)
	{
		return addScalarSliderHelper(label, var, ImGuiDataType_Float, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, uint64_t>::value)
	{
		return addScalarSliderHelper(label, var, ImGuiDataType_U64, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T, double>::value)
	{
		return addScalarSliderHelper(label, var, ImGuiDataType_Double, minVal, maxVal, sameLine, displayFormat);
	}
	else
	{
		static_assert(foobar<T>::value, "Unsupported data type");
	}
}

template<typename T>
bool addVecVarHelper(const std::string& label, T& var, ImGuiDataType_ imguiType, typename T::value_type minValue, typename T::value_type maxValue, float step, bool sameLine, const char* displayFormat)
{
	ImGui::PushItemWidth(200);
	if (sameLine)
	{
		ImGui::SameLine();
	}
	auto b = ImGui::DragScalarN(label.c_str(), imguiType, glm::value_ptr(var), var.length(), step, &minValue, &maxValue, displayFormat);
	var = glm::clamp(var, minValue, maxValue);
	ImGui::PopItemWidth();

	return b;
}


template<typename T>
bool GuiImpl::addVecVar(const std::string& label, T& var, typename T::value_type minVal, typename T::value_type maxVal, float step, bool sameLine, const char* displayFormat)
{
	if constexpr (std::is_same<typename T::value_type, int32_t>::value)
	{
		return addVecVarHelper(label, var, ImGuiDataType_S32, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<typename T::value_type, uint32_t>::value)
	{
		return addVecVarHelper(label, var, ImGuiDataType_U32, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<typename T::value_type, int64_t>::value)
	{
		return addVecVarHelper(label, var, ImGuiDataType_S64, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<typename T::value_type, uint64_t>::value)
	{
		return addVecVarHelper(label, var, ImGuiDataType_U64, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<typename T::value_type, float>::value)
	{
		return addVecVarHelper(label, var, ImGuiDataType_Float, minVal, maxVal, step, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<typename T::value_type, uint64_t>::value)
	{
		return addVecVarHelper(label, var, ImGuiDataType_U64, minVal, maxVal, step, sameLine, displayFormat);
	}
	else
	{
		static_assert(foobar<T>::value, "Unsupported data type");
	}
}

template<typename T>
bool addVecSliderHelper(const std::string& label, T& var, ImGuiDataType_ imguiType, typename T::value_type minValue, typename T::value_type maxValue, bool sameLine, const char* displayFormat)
{
	ImGui::PushItemWidth(200);
	if (sameLine)
	{
		ImGui::SameLine();
	}
	auto b = ImGui::SliderScalarN(label.c_str(), imguiType, glm::value_ptr(var), var.length(), &minValue, &maxValue, displayFormat);
	var = glm::clamp(var, minValue, maxValue);
	ImGui::PopItemWidth();

	return b;
}

template<typename T>
bool GuiImpl::addVecSlider(const std::string& label, T& var, typename T::value_type minVal, typename T::value_type maxVal, bool sameLine, const char* displayFormat)
{
	if constexpr (std::is_same<T::value_type, int32_t>::value)
	{
		return addVecSliderHelper(label, var, ImGuiDataType_S32, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T::value_type, uint32_t>::value)
	{
		return addVecSliderHelper(label, var, ImGuiDataType_U32, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T::value_type, int64_t>::value)
	{
		return addVecSliderHelper(label, var, ImGuiDataType_S64, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T::value_type, uint64_t>::value)
	{
		return addVecSliderHelper(label, var, ImGuiDataType_U64, minVal, maxVal, sameLine, displayFormat);
	}
	else if constexpr (std::is_same<T::value_type, float>::value)
	{
		return addVecSliderHelper(label, var, ImGuiDataType_Float, minVal, maxVal, sameLine, displayFormat);
	}
	else
	{
		static_assert(foobar<T>::value, "Unsupported data type");
	}
}

template<int R, int C, typename T>
void GuiImpl::addMatrixVar(const std::string& label, glm::mat<C, R, T>& var, float minValue, float maxValue, bool sameLine)
{
	std::string labelString{ label };
	std::string hiddenLabelString{ "##" };
	hiddenLabelString += labelString + "[0]";

	auto topLeft = ImGui::GetCursorScreenPos();
	ImVec2 bottomRight;

	auto b = false;

	for (uint32_t i = 0; i < var.length(); i++)
	{
		auto stringToDisplay = hiddenLabelString;
		hiddenLabelString[hiddenLabelString.size() - 2] = '0' + i;

		if (i == var.length() - 1)
		{
			stringToDisplay = labelString;
		}

		b |= addVecVar<C>(stringToDisplay, var[i], minValue, maxValue, 0.001f, sameLine);

		if (i == 0)
		{
			ImGui::SameLine();
			bottomRight = ImGui::GetCursorScreenPos();
			auto oldSpace = ImGui::GetStyle().ItemSpacing.y;

			ImGui::GetStyle().ItemSpacing.y = 0;
			ImGui::Dummy({});
			ImGui::Dummy({});
			ImGui::GetStyle().ItemSpacing.y = oldSpace;

			auto correctedCursorPosition = ImGui::GetCursorScreenPos();
			correctedCursorPosition.y += oldSpace;
			ImGui::GetCursorScreenPos(correctedCursorPosition);
			bottomRight.y = ImGui::GetCursorScreenPos().y;
		}
		else if (i == 1)
		{
			bottomRight.y = topLeft.y + (bottomRight.y - topLeft.y) * (var.length());
			bottomRight.x -= ImGui::GetStyle().ItemInnerSpacing.x * 3 - 1;
			bottomRight.y -= ImGui::GetStyle().ItemInnerSpacing.y - 1;
			topLeft.x -= 1; topLeft.y -= 1;
			auto colourVec4 = ImGui::GetStyleColorVec4(ImGuiCol_ScrollbarGrab); 
			colourVec4.w *= 0.25f;
			auto colour = ImGui::ColorConvertFloat4ToU32(colourVec4);
			ImGui::GetWindowDrawList()->AddRect(topLeft, bottomRight, colour);
		}
	}

	return b;
}

Gui::~Gui()
{
	ImGui::DestroyContext();
}
