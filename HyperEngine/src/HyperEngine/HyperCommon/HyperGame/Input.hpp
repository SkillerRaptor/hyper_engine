#pragma once

#include "KeyCodes.hpp"
#include "MouseCodes.hpp"

#include <HyperMath/Vector2.hpp>

namespace HyperEngine
{
	namespace Input
	{
		enum class CursorInputMode
		{
			None,
			Normal,
			Hidden,
			Disabled
		};
		
		bool IsKeyPressed(KeyCode keyCode);
		bool IsMouseButtonPressed(MouseCode button);
		
		void SetCursorInputMode(CursorInputMode inputMode);
		CursorInputMode GetCursorInputMode();
		
		void SetCursorPosition(Vec2 position);
		Vec2 GetCursorPosition();
	};
}
