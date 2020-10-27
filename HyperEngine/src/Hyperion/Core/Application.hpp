#pragma once

#include <queue>

#include "Core.hpp"
#include "Events/Event.hpp"
#include "HyperLayer/HyperLayer.hpp"
#include "Rendering/Window.hpp"

namespace Hyperion 
{
	class Application
	{
	private:
		Window* m_Window;
		LayerStack* m_LayerStack;
		bool m_Running = true;

		std::queue<Ref<Event>> m_EventBus;

		Ref<Scene> m_Scene;

		static Application* m_Instance;

	public:
		Application();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void Run();

		Window* GetNativeWindow() const;
		static Application* Get();

	private:
		void OnEvent(Event& event);
		void Shutdown();
	};

	Application* CreateApplication();
}
