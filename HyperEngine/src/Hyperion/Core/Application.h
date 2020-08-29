#pragma once

#include "Rendering/Window.h"

namespace Hyperion 
{
	class Application
	{
	private:
		Window* m_Window;
		bool m_Running = true;

		static Application* m_Instance;

	public:
		Application();
		virtual ~Application();

		void Run();

		Window* GetWindow() const;
		static Application* Get();

	private:
		void Shutdown();
	};

	Application* CreateApplication();
}