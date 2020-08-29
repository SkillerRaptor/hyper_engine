#pragma once

#include <queue>

#include "Events/Event.h"
#include "Rendering/Window.h"

namespace Hyperion 
{
	class Application
	{
	private:
		Window* m_Window;
		bool m_Running = true;

		std::queue<std::shared_ptr<Event>> m_EventBus;

		static Application* m_Instance;

	public:
		Application();

		void Run();

		Window* GetWindow() const;
		static Application* Get();

	private:
		void OnEvent(Event& event);
		void Shutdown();
	};

	Application* CreateApplication();
}