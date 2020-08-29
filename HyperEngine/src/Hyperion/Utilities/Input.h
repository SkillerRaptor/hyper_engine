#pragma once

namespace Hyperion
{
	class Input
	{
	public:
		static bool IsKeyUp(int keyCode);
		static bool IsKeyPressed(int keyCode);
		static bool IsKeyDown(int keyCode);

		static float GetAxis(const char* axis);

		static bool IsMouseButtonPressed(int button);
		static float GetMouseX();
		static float GetMouseY();
	};
}