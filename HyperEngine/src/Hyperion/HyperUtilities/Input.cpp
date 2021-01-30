#include "Input.hpp"

#include <GLFW/glfw3.h>

#include "HyperCore/Application.hpp"

namespace Hyperion
{
	int Input::GetKeyState(KeyCode keyCode)
	{
		return glfwGetKey(GetWindow(), keyCode);
	}

	bool Input::IsKeyUp(KeyCode keyCode)
	{
		int state = GetKeyState(keyCode);
		return state == GLFW_PRESS;
	}

	bool Input::IsKeyPressed(KeyCode keyCode)
	{
		int state = GetKeyState(keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyDown(KeyCode keyCode)
	{
		int state = GetKeyState(keyCode);
		return state == GLFW_RELEASE;
	}

	float Input::GetAxis(InputAxis axis)
	{
		switch (axis)
		{
		case InputAxis::HORIZONTAL:
			if (IsKeyPressed(KeyCode::A) || IsKeyPressed(KeyCode::Left))
				return 1;
			if (IsKeyPressed(KeyCode::D) || IsKeyPressed(KeyCode::Right))
				return -1;
			break;
		case InputAxis::VERTICAL:
			if (IsKeyPressed(KeyCode::W) || IsKeyPressed(KeyCode::Up))
				return 1;
			if (IsKeyPressed(KeyCode::S) || IsKeyPressed(KeyCode::Down))
				return -1;
			break;
		}
		return 0;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		int state = glfwGetMouseButton(GetWindow(), button);
		return state == GLFW_PRESS;
	}

	float Input::GetMouseX()
	{
		double xPos, yPos;
		glfwGetCursorPos(GetWindow(), &xPos, &yPos);
		return static_cast<float>(xPos);
	}

	float Input::GetMouseY()
	{
		double xPos, yPos;
		glfwGetCursorPos(GetWindow(), &xPos, &yPos);
		return static_cast<float>(yPos);
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		double xPos, yPos;
		glfwGetCursorPos(GetWindow(), &xPos, &yPos);
		return { static_cast<float>(xPos), static_cast<float>(yPos) };
	}

	GLFWwindow* Input::GetWindow()
	{
		return Application::Get()->GetWindow()->GetContext()->GetWindow();
	}
}
