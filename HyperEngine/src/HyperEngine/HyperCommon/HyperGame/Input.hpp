#pragma once

#include "KeyCodes.hpp"
#include "MouseCodes.hpp"

#include <glm/glm.hpp>

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
		
		void SetCursorPosition(glm::vec2 position); // TODO: Replace GLM with HyperMath
		glm::vec2 GetCursorPosition(); // TODO: Replace GLM with HyperMath
	};
}
