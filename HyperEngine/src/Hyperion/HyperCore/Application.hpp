#pragma once

#include <queue>

#include "HyperCore/Core.hpp"
#include "HyperECS/Scene/Scene.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperLayer/Layer.hpp"
#include "HyperLayer/LayerStack.hpp"
#include "HyperLayer/OverlayLayer.hpp"
#include "HyperRendering/Window.hpp"

namespace Hyperion 
{
	class Application
	{
	private:
		Ref<Window> m_Window;
		Scope<LayerStack> m_LayerStack;
		bool m_Running = true;

		std::queue<Ref<Event>> m_EventBus;

		Ref<Scene> m_Scene;

		static Application* m_Instance;

	public:
		Application();

		void Run();

		void PushLayer(Layer* layer);
		void PushLayer(OverlayLayer* overlayLayer);

		void PopLayer(Layer* layer);
		void PopLayer(OverlayLayer* overlayLayer);

		void PopLayer(const std::string& layerName);
		void PopOverlayLayer(const std::string& layerName);

		void PopLastLayer();
		void PopLastOverlayLayer();

		const Layer* GetLayer(const std::string& layerName) const;
		const OverlayLayer* GetOverlayLayer(const std::string& layerName) const;

		Ref<Window> GetNativeWindow() const;
		static Application* Get();

	private:
		void Init();
		void Shutdown();
		void OnEvent(Event& event);
	};

	Application* CreateApplication();
}
