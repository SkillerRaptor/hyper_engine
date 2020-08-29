#include "Input.h"

#include "Core/Application.h"

namespace Hyperion
{
	bool Input::IsKeyUp(int keyCode)
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS;
	}

	bool Input::IsKeyPressed(int keyCode)
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyDown(int keyCode)
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_RELEASE;
	}

	float Input::GetAxis(const char* axis)
	{
		GLFWwindow* window = Application::Get()->GetWindow()->GetNativeWindow();
		if (axis == "Horizontal")
		{
			if (IsKeyPressed(GLFW_KEY_A) || IsKeyPressed(GLFW_KEY_LEFT))
				return 1;
			if (IsKeyPressed(GLFW_KEY_D) || IsKeyPressed(GLFW_KEY_RIGHT))
				return -1;
		}
		else if (axis == "Vertical")
		{
			if (IsKeyPressed(GLFW_KEY_W) || IsKeyPressed(GLFW_KEY_UP))
				return 1;
			if (IsKeyPressed(GLFW_KEY_S) || IsKeyPressed(GLFW_KEY_DOWN))
				return -1;
		}
		return 0;
	}

	bool Input::IsMouseButtonPressed(int button)
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