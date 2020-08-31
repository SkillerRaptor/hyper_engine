#include "Input.h"

#include "Core/Application.h"

namespace Hyperion
{
	bool Input::IsKeyUp(KeyCode keyCode)
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS;
	}

	bool Input::IsKeyPressed(KeyCode keyCode)
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyDown(KeyCode keyCode)
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_RELEASE;
	}

	float Input::GetAxis(InputAxis axis)
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
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
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		int state = glfwGetKey(window, button);
		return state == GLFW_PRESS;
	}

	float Input::GetMouseX()
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return (float)xPos;
	}

	float Input::GetMouseY()
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return (float)yPos;
	}
}