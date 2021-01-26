#pragma once

#include <glm/glm.hpp>

#include <string>

namespace Hyperion::PanelUtilities
{
	void DrawCheckbox(const std::string& title, bool& value);

	void DrawColorEdit3(const std::string& title, glm::vec3& value);
	void DrawColorEdit4(const std::string& title, glm::vec4& value);

	void DrawDragInt(const std::string& title, int& value, int speed = 1, int min = 0, int max = 0);
	void DrawDragUnsignedInt(const std::string& title, uint32_t& value, int speed = 1, int max = 0);
	void DrawDragFloat(const std::string& title, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void DrawDragVec2(const std::string& title, glm::vec2& vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
	void DrawDragVec3(const std::string& title, glm::vec3& vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
	void DrawDragVec4(const std::string& title, glm::vec4& vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void DrawSelection();
}
