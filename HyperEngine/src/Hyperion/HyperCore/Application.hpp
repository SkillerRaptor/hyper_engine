#pragma once

#include <string>
#include <queue>

#include "HyperCore/Core.hpp"
#include "HyperECS/Scene/Scene.hpp"
#include "HyperEvents/Event.hpp"
#include "HyperLayer/Layer.hpp"
#include "HyperLayer/LayerStack.hpp"
#include "HyperLayer/OverlayLayer.hpp"
#include "HyperRendering/ImGuiLayer.hpp"
#include "HyperRendering/SceneRecorder.hpp"
#include "HyperRendering/Window.hpp"

namespace Hyperion 
{
	class Application
	{
	private:
		Ref<Window> m_Window;

		std::queue<Ref<Event>> m_EventBus;

		Scope<LayerStack> m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		Ref<Scene> m_Scene;

		bool m_Running = true;

		static Application* m_Instance;

	public:
		Application(const std::string& title, uint32_t width, uint32_t height);

		void Run();

		void SetAppIcon(const std::string& imagePath);

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

		void SetScene(const Ref<Scene>& scene);
		const Ref<Scene>& GetScene() const;

		const Ref<Window>& GetWindow() const;
		static Application* Get();

	private:
		void Init(const std::string& title, uint32_t width, uint32_t height);
		void Shutdown();

		void OnEvent(Event& event);
	};

	Application* CreateApplication();
}
