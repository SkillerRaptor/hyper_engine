#pragma once

#include <iostream>
#include <queue>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Events/Event.h"

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
		std::queue<std::shared_ptr<Event>>* EventBus;
	};

	class Window
	{
	private:
		GLFWwindow* m_Window;
		WindowData m_Data;

	public:
		Window(std::string title = "HyperEngine", unsigned int width = 1280, unsigned int height = 720, bool vSync = false, std::queue<std::shared_ptr<Event>>* eventBus = nullptr);
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

		GLFWwindow* GetNativeWindow() const;
		const WindowData& GetWindowData() const;

	private:
		void InitWindow(std::string title, int width, int height, bool vSync, std::queue<std::shared_ptr<Event>>* eventBus);
		void ShutdownWindow();
	};
}