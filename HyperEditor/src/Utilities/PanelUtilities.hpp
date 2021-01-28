#pragma once

#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace Hyperion::PanelUtilities
{
	void DrawCheckbox(const std::string& title, bool& value);

	void DrawColorEdit3(const std::string& title, glm::vec3& value);
	void DrawColorEdit4(const std::string& title, glm::vec4& value);

	void DrawDragInt(const std::string& title, int& value, int speed = 1, int min = 0, int max = 0);
	void DrawDragUnsignedInt(const std::string& title, uint8_t& value, int speed = 1, int max = 0);
	void DrawDragUnsignedInt(const std::string& title, uint16_t& value, int speed = 1, int max = 0);
	void DrawDragUnsignedInt(const std::string& title, uint32_t& value, int speed = 1, int max = 0);
	void DrawDragUnsignedInt(const std::string& title, uint64_t& value, int speed = 1, int max = 0);
	void DrawDragFloat(const std::string& title, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void DrawDragVec2(const std::string& title, glm::vec2& vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const std::array<std::string, 2>& = { "X", "Y" });
	void DrawDragVec3(const std::string& title, glm::vec3& vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const std::array<std::string, 3>& = { "X", "Y", "Z" });
	void DrawDragVec4(const std::string& title, glm::vec4& vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const std::array<std::string, 4>& = { "X", "Y", "Z", "W" });

	void DrawCombo(const std::string& title, std::string& currentItem, const std::vector<std::string>& items);
	void DrawInputCombo(const std::string& title, std::string& currentItem, const std::vector<std::string>& items);

	void DrawSelectableImage(const std::string& title, uint32_t imageId, const typename std::common_type<std::function<void()>>::type clickFunction, const typename std::common_type<std::function<void()>>::type removeFunction);

	void SplitNode(const std::string& title, typename std::common_type<std::function<void()>>::type function);

	void DrawSelection();
	void DrawRectAroundWindow(const glm::vec4& color);
}
