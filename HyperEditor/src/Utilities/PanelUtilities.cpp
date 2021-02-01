#include "PanelUtilities.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace PanelUtilities
{
	rttr::variant DrawCheckbox(const std::string& title, bool value)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::Checkbox(std::string("##" + title).c_str(), &value);

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawColorEdit3(const std::string& title, glm::vec3 value)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::ColorEdit3(std::string("##" + title).c_str(), glm::value_ptr(value));

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawColorEdit4(const std::string& title, glm::vec4 value)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::ColorEdit4(std::string("##" + title).c_str(), glm::value_ptr(value));

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawDragInt(const std::string& title, int value, int speed, int min, int max)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::DragInt(std::string("##" + title).c_str(), (int*)&value, static_cast<float>(speed), min, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawDragUnsignedInt(const std::string& title, uint8_t value, int speed, int max)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::DragInt(std::string("##" + title).c_str(), (int*)&value, static_cast<float>(speed), 0, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawDragUnsignedInt(const std::string& title, uint16_t value, int speed, int max)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::DragInt(std::string("##" + title).c_str(), (int*)&value, static_cast<float>(speed), 0, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawDragUnsignedInt(const std::string& title, uint32_t value, int speed, int max)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::DragInt(std::string("##" + title).c_str(), (int*)&value, static_cast<float>(speed), 0, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawDragUnsignedInt(const std::string& title, uint64_t value, int speed, int max)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::DragInt(std::string("##" + title).c_str(), (int*)&value, static_cast<float>(speed), 0, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawDragFloat(const std::string& title, float value, float speed, float min, float max)
	{
		bool edited = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		edited = ImGui::DragFloat(std::string("##" + title).c_str(), &value, speed, min, max, "%.2f", 1.0f);

		ImGui::Columns(1);

		ImGui::PopID();

		return edited ? rttr::variant{ value } : rttr::variant{};
	}

	rttr::variant DrawDragVec2(const std::string& title, glm::vec2 vector, float speed, float min, float max, const std::array<std::string, 2>& labels)
	{
		bool editedX = false;
		bool editedY = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

		editedX = ImGui::DragFloat(std::string("##" + title + "X").c_str(), &vector.x, speed, min, max, (labels[0] + ": %.2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		editedY = ImGui::DragFloat(std::string("##" + title + "Y").c_str(), &vector.y, speed, min, max, (labels[1] + ": %.2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();

		return (editedX || editedY) ? rttr::variant{ vector } : rttr::variant{};
	}

	rttr::variant DrawDragVec3(const std::string& title, glm::vec3 vector, float speed, float min, float max, const std::array<std::string, 3>& labels)
	{
		bool editedX = false;
		bool editedY = false;
		bool editedZ = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		editedX = ImGui::DragFloat(std::string("##" + title + "X").c_str(), &vector.x, speed, min, max, (labels[0] + ": %.2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		editedY = ImGui::DragFloat(std::string("##" + title + "Y").c_str(), &vector.y, speed, min, max, (labels[1] + ": %.2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		editedZ = ImGui::DragFloat(std::string("##" + title + "Z").c_str(), &vector.z, speed, min, max, (labels[2] + ": %.2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();

		return (editedX || editedY || editedZ) ? rttr::variant{ vector } : rttr::variant{};
	}

	rttr::variant DrawDragVec4(const std::string& title, glm::vec4 vector, float speed, float min, float max, const std::array<std::string, 4>& labels)
	{
		bool editedX = false;
		bool editedY = false;
		bool editedZ = false;
		bool editedW = false;

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());

		editedX = ImGui::DragFloat(std::string("##" + title + "X").c_str(), &vector.x, speed, min, max, (labels[0] + ": %.2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		editedY = ImGui::DragFloat(std::string("##" + title + "Y").c_str(), &vector.y, speed, min, max, (labels[1] + ": %.2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		editedZ = ImGui::DragFloat(std::string("##" + title + "Z").c_str(), &vector.z, speed, min, max, (labels[2] + ": %.2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		editedW = ImGui::DragFloat(std::string("##" + title + "W").c_str(), &vector.w, speed, min, max, (labels[3] + ": % .2f").c_str(), 1.0f);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();

		return (editedX || editedY || editedZ || editedW) ? rttr::variant{ vector } : rttr::variant{};
	}

	rttr::variant DrawCombo(const std::string& title, const rttr::variant& v)
	{
		rttr::enumeration e = v.get_type().get_enumeration();
		const char* currentItem = e.value_to_name(v).cbegin();

		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);

		rttr::variant value{};
		if (ImGui::BeginCombo("##Combo", currentItem))
		{
			for (const rttr::string_view& enumName : e.get_names())
			{
				rttr::variant enumValue = e.name_to_value(enumName);
				if (ImGui::Selectable(enumName.cbegin(), enumValue == v))
				{
					ImGui::SetItemDefaultFocus();
					value = enumValue;
					break;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Columns(1);

		ImGui::PopID();

		return value;
	}

	rttr::variant DrawInputCombo(const std::string& title, const rttr::variant& v)
	{
		/*
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		currentItem.copy(buffer, sizeof(buffer));

		if (ImGui::InputText("##Input", buffer, sizeof(buffer)))
			currentItem = std::string(buffer).empty() ? "" : std::string(buffer);

		ImGui::SameLine();

		static bool isOpen = false;
		bool isFocused = ImGui::IsItemFocused();
		isOpen |= ImGui::IsItemActive();
		if (isOpen)
		{
			ImGui::SetNextWindowPos({ ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y });
			ImGui::SetNextWindowSize({ ImGui::GetItemRectSize().x, 0 });

			if (ImGui::Begin("##Popup", &isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Tooltip))
			{
				ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
				isFocused |= ImGui::IsWindowFocused();
				for (int i = 0; i < items.size(); i++)
				{
					std::string item = items[i];
					if (!currentItem.empty() && item.find(currentItem) == std::string::npos)
						continue;

					if (ImGui::Selectable(item.c_str()) || (ImGui::IsItemFocused() && ImGui::IsKeyPressedMap(ImGuiKey_Enter)))
					{
						currentItem = item;
						isOpen = false;
						edited = true;
					}
				}
			}
			ImGui::End();

			isOpen &= isFocused;
		}

		ImGui::Columns(1);

		ImGui::PopID();
		*/
		return rttr::variant{};
	}

	void DrawSelectableImage(const std::string& title, uint32_t imageId, const typename std::common_type<std::function<void()>>::type clickFunction, const typename std::common_type<std::function<void()>>::type removeFunction)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		static const ImVec2 buttonSize = { 70, 70 };
		if (ImGui::ImageButton((ImTextureID)(uintptr_t)imageId, buttonSize, { 0, 1 }, { 1, 0 }, 0))
			clickFunction();
		ImGui::PopItemWidth();

		ImGui::SameLine();

		if (ImGui::Button("x", { buttonSize.x / 4, buttonSize.y }))
			removeFunction();
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SplitNode(const std::string& title, typename std::common_type<std::function<void()>>::type function)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		if (ImGui::TreeNodeEx((void*)std::hash<std::string>()(title), ImGuiTreeNodeFlags_DefaultOpen, title.c_str()))
		{
			function();
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}

	void DrawSelection()
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.x -= 5.0f;
		pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.6f;
		ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
		ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false, 5.0f);
	}

	void DrawRectAroundWindow(const glm::vec4& color)
	{
		ImVec2 windowMin = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 windowMax = { windowMin.x + windowSize.x, windowMin.y + windowSize.y };
		ImGui::GetForegroundDrawList()->AddRect(windowMin, windowMax, ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, color.w)));
	}
}
