#pragma once

#include "hppch.h"

#include "Rendering/Window.h"

namespace Hyperion {

	class Application
	{
	private:
		Window* m_Window;
		bool m_Running = true;

	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		void Shutdown();
	};

	Application* CreateApplication();
}