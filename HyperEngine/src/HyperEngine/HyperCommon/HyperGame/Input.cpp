#include "Input.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperEngine
{
	namespace Input
	{
		bool IsKeyPressed(KeyCode keyCode)
		{
			GLFWwindow* pWindow{ nullptr };
			const int state{ glfwGetKey(pWindow, keyCode) };
			return state == GLFW_PRESS || state == GLFW_REPEAT;
		}
		
		bool IsMouseButtonPressed(MouseCode button)
		{
			GLFWwindow* pWindow{ nullptr };
			const int state{ glfwGetMouseButton(pWindow, button) };
			return state == GLFW_PRESS;
		}
		
		void SetCursorInputMode(CursorInputMode inputMode)
		{
			GLenum cursorInputMode{ GLFW_CURSOR_NORMAL };
			switch (inputMode)
			{
			case CursorInputMode::Normal:
				cursorInputMode = GLFW_CURSOR_NORMAL;
				break;
			case CursorInputMode::Hidden:
				cursorInputMode = GLFW_CURSOR_HIDDEN;
				break;
			case CursorInputMode::Disabled:
				cursorInputMode = GLFW_CURSOR_DISABLED;
				break;
			default:
				break;
			}
			
			GLFWwindow* pWindow{ nullptr };
			glfwSetInputMode(pWindow, GLFW_CURSOR, cursorInputMode);
		}
		
		CursorInputMode GetCursorInputMode()
		{
			GLFWwindow* pWindow{ nullptr };
			
			const GLenum cursorInputMode{ static_cast<GLenum>(glfwGetInputMode(pWindow, GLFW_CURSOR)) };
			switch (cursorInputMode)
			{
			case GLFW_CURSOR_NORMAL:
				return CursorInputMode::Normal;
			case GLFW_CURSOR_HIDDEN:
				return CursorInputMode::Hidden;
			case GLFW_CURSOR_DISABLED:
				return CursorInputMode::Disabled;
			default:
				return CursorInputMode::None;
			}
		}
		
		void SetCursorPosition(glm::vec2 position)
		{
			GLFWwindow* pWindow{ nullptr };
			glfwSetCursorPos(pWindow, static_cast<float>(position.x), static_cast<float>(position.y));
		}
		
		glm::vec2 GetCursorPosition()
		{
			GLFWwindow* pWindow{ nullptr };
			
			double cursorPositionX{ 0.0 };
			double cursorPositionY{ 0.0 };
			glfwGetCursorPos(pWindow, &cursorPositionX, &cursorPositionY);
			
			return { static_cast<float>(cursorPositionX), static_cast<float>(cursorPositionY) };
		}
	}
}
