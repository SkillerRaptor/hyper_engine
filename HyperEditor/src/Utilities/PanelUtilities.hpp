#pragma once

#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <string>
#include <vector>

#include <Hyperion.hpp>

using namespace Hyperion;

class PanelUtilities
{
private:
	static Ref<RenderContext> s_RenderContext;

public:
	static rttr::variant DrawCheckbox(const std::string& title, bool value);

	static rttr::variant DrawColorEdit3(const std::string& title, glm::vec3 value);
	static rttr::variant DrawColorEdit4(const std::string& title, glm::vec4 value);

	static rttr::variant DrawDragInt(const std::string& title, int value, int speed = 1, int min = 0, int max = 0);
	static rttr::variant DrawDragUnsignedInt(const std::string& title, uint8_t value, int speed = 1, int max = 0);
	static rttr::variant DrawDragUnsignedInt(const std::string& title, uint16_t value, int speed = 1, int max = 0);
	static rttr::variant DrawDragUnsignedInt(const std::string& title, uint32_t value, int speed = 1, int max = 0);
	static rttr::variant DrawDragUnsignedInt(const std::string& title, uint64_t value, int speed = 1, int max = 0);
	static rttr::variant DrawDragFloat(const std::string& title, float value, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	static rttr::variant DrawDragVec2(const std::string& title, glm::vec2 vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const std::array<std::string, 2> & = { "X", "Y" });
	static rttr::variant DrawDragVec3(const std::string& title, glm::vec3 vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const std::array<std::string, 3> & = { "X", "Y", "Z" });
	static rttr::variant DrawDragVec4(const std::string& title, glm::vec4 vector, float speed = 0.1f, float min = 0.0f, float max = 0.0f, const std::array<std::string, 4> & = { "X", "Y", "Z", "W" });

	static rttr::variant DrawCombo(const std::string& title, const rttr::variant& v);
	static rttr::variant DrawInputCombo(const std::string& title, const rttr::variant& v);

	static rttr::variant DrawSelectableImage(const std::string& title, TextureHandle texture);

	static void SplitNode(const std::string& title, typename std::common_type<std::function<void()>>::type function);

	static void DrawSelection();
	static void DrawRectAroundWindow(const glm::vec4& color);

	static void SetRenderContext(Ref<RenderContext> renderContext);
};
