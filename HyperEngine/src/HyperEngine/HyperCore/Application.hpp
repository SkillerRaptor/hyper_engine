#pragma once

#include <string>
#include <queue>

#include "HyperCore/Core.hpp"
#include "HyperEvent/Event.hpp"
#include "HyperLayer/Layer.hpp"
#include "HyperLayer/LayerStack.hpp"
#include "HyperLayer/OverlayLayer.hpp"
#include "HyperRendering/ImGuiLayer.hpp"
#include "HyperRendering/Scene.hpp"
#include "HyperRendering/SceneRecorder.hpp"
#include "HyperRendering/Window.hpp"

namespace HyperCore
{
	class Application
	{
	private:
		Ref<HyperRendering::Window> m_Window;

		HyperEvent::EventManager m_EventManager;

		Scope<HyperLayer::LayerStack> m_LayerStack;
		HyperRendering::ImGuiLayer* m_ImGuiLayer;

		Ref<HyperRendering::Scene> m_Scene;

		bool m_Running = true;

		static Application* m_Instance;

	public:
		Application(const std::string& title, uint32_t width, uint32_t height);
		virtual ~Application() = default;

		void Run();

		void SetAppIcon(const std::string& imagePath);

		inline void PushLayer(HyperLayer::Layer* layer)
		{
			layer->SetRenderContext(m_Window->GetContext());
			layer->SetScene(m_Scene);
			layer->RegisterEvents(m_EventManager);
			m_LayerStack->PushLayer(layer);
		}

		inline void PushLayer(HyperLayer::OverlayLayer* overlayLayer)
		{
			overlayLayer->SetRenderContext(m_Window->GetContext());
			overlayLayer->SetScene(m_Scene);
			overlayLayer->RegisterEvents(m_EventManager);
			m_LayerStack->PushLayer(overlayLayer);
		}

		inline void PopLayer(HyperLayer::Layer* layer)
		{
			m_LayerStack->PopLayer(layer);
		}

		inline void PopLayer(HyperLayer::OverlayLayer* overlayLayer)
		{
			m_LayerStack->PopLayer(overlayLayer);
		}

		inline void PopLayer(const std::string& layerName)
		{
			m_LayerStack->PopLayer(layerName);
		}

		inline void PopOverlayLayer(const std::string& overlayLayerName)
		{
			m_LayerStack->PopOverlayLayer(overlayLayerName);
		}

		inline void PopLastLayer()
		{
			m_LayerStack->PopLastLayer();
		}

		inline void PopLastOverlayLayer()
		{
			m_LayerStack->PopLastOverlayLayer();
		}

		inline const HyperLayer::Layer* GetLayer(const std::string& layerName) const
		{
			return m_LayerStack->GetLayer(layerName);
		}

		inline const HyperLayer::OverlayLayer* GetOverlayLayer(const std::string& overlayLayerName) const
		{
			return m_LayerStack->GetOverlayLayer(overlayLayerName);
		}

		inline void SetScene(const Ref<HyperRendering::Scene>& scene)
		{
			m_Scene = scene;
		}

		inline const Ref<HyperRendering::Scene>& GetScene() const
		{
			return m_Scene;
		}

		inline const Ref<HyperRendering::Window>& GetWindow() const
		{
			return m_Window;
		}

		static inline Application* Get()
		{
			return m_Instance;
		}

	private:
		void Init(const std::string& title, uint32_t width, uint32_t height);
		void Shutdown();
	};

	Application* CreateApplication();
}
