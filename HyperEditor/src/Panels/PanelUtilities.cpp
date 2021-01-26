#include "PanelUtilities.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace Hyperion::PanelUtilities
{
	void DrawCheckbox(const std::string& title, bool& value)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::Checkbox(std::string("##" + title).c_str(), &value);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawColorEdit3(const std::string& title, glm::vec3& value)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::ColorEdit3(std::string("##" + title).c_str(), glm::value_ptr(value));

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawColorEdit4(const std::string& title, glm::vec4& value)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::ColorEdit4(std::string("##" + title).c_str(), glm::value_ptr(value));

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawDragInt(const std::string& title, int& value, int speed, int min, int max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::DragInt(std::string("##" + title).c_str(), (int*)&value, static_cast<float>(speed), min, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawDragUnsignedInt(const std::string& title, uint32_t& value, int speed, int max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::DragInt(std::string("##" + title).c_str(), (int*)&value, static_cast<float>(speed), 0, max, "%.2f", 0);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawDragFloat(const std::string& title, float& value, float speed, float min, float max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::DragFloat(std::string("##" + title).c_str(), &value, speed, min, max, "%.2f", 1.0f);

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawDragVec2(const std::string& title, glm::vec2& vector, float speed, float min, float max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

		ImGui::DragFloat(std::string("##" + title + "X").c_str(), &vector.x, speed, min, max, "X: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat(std::string("##" + title + "Y").c_str(), &vector.y, speed, min, max, "Y: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawDragVec3(const std::string& title, glm::vec3& vector, float speed, float min, float max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		ImGui::DragFloat(std::string("##" + title + "X").c_str(), &vector.x, speed, min, max, "X: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat(std::string("##" + title + "Y").c_str(), &vector.y, speed, min, max, "Y: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat(std::string("##" + title + "Z").c_str(), &vector.z, speed, min, max, "Z: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawDragVec4(const std::string& title, glm::vec4& vector, float speed, float min, float max)
	{
		ImGui::PushID(title.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 150.0f);
		ImGui::Text(title.c_str());
		ImGui::NextColumn();

		ImGui::SetNextItemWidth(-1);
		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());

		ImGui::DragFloat(std::string("##" + title + "X").c_str(), &vector.x, speed, min, max, "X: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat(std::string("##" + title + "Y").c_str(), &vector.y, speed, min, max, "Y: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat(std::string("##" + title + "Z").c_str(), &vector.z, speed, min, max, "Z: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::DragFloat(std::string("##" + title + "W").c_str(), &vector.w, speed, min, max, "W: %.2f", 1.0f);
		ImGui::PopItemWidth();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawSelection()
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		pos.x -= 5.0f;
		pos.y -= ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.6f;
		ImU32 col = ImColor(ImVec4(0.70f, 0.70f, 0.70f, 0.40f));
		ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + ImGui::GetTextLineHeight() + ImGui::GetTextLineHeight() * 0.25f), col, false, 5.0f);
	}
}
