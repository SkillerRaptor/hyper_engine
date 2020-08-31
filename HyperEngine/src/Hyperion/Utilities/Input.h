#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Hyperion
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
	};
}