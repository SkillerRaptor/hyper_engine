#include "Input.hpp"

#include <GLFW/glfw3.h>

#include "HyperCore/Application.hpp"

namespace Hyperion
{
	bool Input::IsKeyUp(KeyCode keyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetWindow());
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS;
	}

	bool Input::IsKeyPressed(KeyCode keyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetWindow());
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyDown(KeyCode keyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetWindow());
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_RELEASE;
	}

	float Input::GetAxis(InputAxis axis)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetWindow());
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
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetWindow());
		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	float Input::GetMouseX()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return (float)xPos;
	}

	float Input::GetMouseY()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return (float)yPos;
	}
}
