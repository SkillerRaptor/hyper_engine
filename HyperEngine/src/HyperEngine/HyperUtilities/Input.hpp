#pragma once

#include <utility>

#include "HyperUtilities/KeyCodes.hpp"
#include "HyperUtilities/MouseCodes.hpp"

struct GLFWwindow;

namespace HyperUtilities
{
	enum class InputAxis
	{
		HORIZONTAL,
		VERTICAL
	};

	class Input
	{
	public:
		static bool IsKeyUp(KeyCode keyCode);
		static bool IsKeyPressed(KeyCode keyCode);
		static bool IsKeyDown(KeyCode keyCode);

		static float GetAxis(InputAxis axis);

		static bool IsMouseButtonPressed(MouseCode button);
		static float GetMouseX();
		static float GetMouseY();
		static std::pair<float, float> GetMousePosition();

	private:
		static int GetKeyState(KeyCode keyCode);
		static GLFWwindow* GetWindow();
	};
}
