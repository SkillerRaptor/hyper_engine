#pragma once

#include "hppch.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hyperion
{
	struct WindowData
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		unsigned int XPos;
		unsigned int YPos;

		bool VSync;
	};

	class Window
	{
	private:
		GLFWwindow* m_Window;
		WindowData m_Data;

	public:
		Window(std::string title = "HyperEngine", unsigned int width = 1280, unsigned int height = 720, bool vSync = false);
		virtual ~Window();

		void OnUpdate();

		void SetTitle(std::string title);
		std::string GetTitle() const;

		void SetWidth(int width);
		int GetWidth() const;

		void SetHeight(int height);
		int GetHeight() const;

		void SetXPos(int xPos);
		int GetXPos() const;

		void SetYPos(int yPos);
		int GetYPos() const;

		void SetVSync(bool vSync);
		bool IsVSync() const;

		GLFWwindow* GetWindow() const;
		const WindowData& GetWindowData() const;

	private:
		void InitWindow(std::string title, int width, int height, bool vSync);
		void ShutdownWindow();
	};
}