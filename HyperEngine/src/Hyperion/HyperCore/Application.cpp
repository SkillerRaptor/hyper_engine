#include "Application.hpp"

#include "HyperECS/Components/Components.hpp"
#include "HyperEvents/WindowEvents.hpp"
#include "HyperRendering/ImGuiLayer.hpp"
#include "HyperUtilities/Random.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace Hyperion
{
	Application* Application::m_Instance;

	Application::Application(const std::string& title, uint32_t width, uint32_t height)
	{
		Init(title, width, height);
	}

	void Application::Init(const std::string& title, uint32_t width, uint32_t height)
	{
		m_Instance = this;

		Random::Init();

		HP_CORE_INFO("");
		HP_CORE_INFO("_     _ __   __  _____  _______  ______ _______ __   _  ______ _____ __   _ _______");
		HP_CORE_INFO("|_____|   \\_/   |_____] |______ |_____/ |______ | \\  | |  ____   |   | \\  | |______");
		HP_CORE_INFO("|     |    |    |       |______ |    \\_ |______ |  \\_| |_____| __|__ |  \\_| |______");
		HP_CORE_INFO("");

		WindowPropsInfo windowPropsInfo{};
		windowPropsInfo.Title = title;
		windowPropsInfo.Width = width;
		windowPropsInfo.Height = height;
		windowPropsInfo.EventBus = &m_EventBus;

		m_Window = Window::Construct(windowPropsInfo);

		m_Scene = CreateRef<Scene>("Main Scene", m_Window->GetContext()->GetRenderer2D());

		m_LayerStack = CreateScope<LayerStack>();
		m_ImGuiLayer = new ImGuiLayer();
		PushLayer(m_ImGuiLayer);
	}

	void Application::Shutdown()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		double lastTime = 0.0;

		Ref<SceneRecorder> sceneRecorder = m_Window->GetContext()->GetSceneRecorder();
		bool enableCapture = m_LayerStack->GetOverlayLayers().size() > 1;

		if (enableCapture)
			sceneRecorder->InitRecorder();

		while (m_Running)
		{
			double currentTime = m_Window->GetTime();
			const Timestep timeStep = currentTime - lastTime;
			lastTime = currentTime;

			m_Window->OnPreUpdate();

			while (!m_EventBus.empty())
			{
				Ref<Event> e = m_EventBus.front();
				if (e->Handled) continue;
				OnEvent(*e);
				m_EventBus.pop();
			}

			bool validCamera = false;
			TransformComponent transformComponent;
			CameraComponent cameraComponent;
			for (Entity entity : m_Scene->GetWorld().GetEntities<CameraComponent>())
			{
				CameraComponent& entityCameraComponent = m_Scene->GetWorld().GetComponent<CameraComponent>(entity);
				if (!entityCameraComponent.IsPrimary())
					continue;

				validCamera = true;
				transformComponent = m_Scene->GetWorld().GetComponent<TransformComponent>(entity);
				cameraComponent = entityCameraComponent;
				break;
			}

			if (enableCapture)
			{
				sceneRecorder->RebuildRecoder();
				sceneRecorder->StartGameRecording();
			}

			if (validCamera)
			{
				m_Window->GetContext()->GetRenderer2D()->SetCamera(transformComponent.GetPosition(), transformComponent.GetRotation(), cameraComponent.GetFieldOfView(), cameraComponent.GetClippingPlanes(), cameraComponent.GetViewportRect(), cameraComponent.GetProjection());

				for (Layer* layer : m_LayerStack->GetLayers())
				{
					layer->OnUpdate(timeStep);
					layer->OnRender();
				}

				m_Scene->OnUpdate(timeStep);
				m_Scene->OnRender();
			}

			if (enableCapture)
			{
				sceneRecorder->EndGameRecording();

				EditorCamera& editorCamera = m_ImGuiLayer->GetEditorCamera();
				m_Window->GetContext()->GetRenderer2D()->SetCamera(editorCamera.GetProjectionMatrix(), editorCamera.GetViewMatrix());

				sceneRecorder->StartEditorRecording();

				for (Layer* layer : m_LayerStack->GetLayers())
				{
					layer->OnUpdate(timeStep);
					layer->OnRender();
				}

				m_Scene->OnUpdate(timeStep);
				m_Scene->OnRender();

				sceneRecorder->EndEditorRecording();

				m_ImGuiLayer->Start();
				for (OverlayLayer* overlayLayer : m_LayerStack->GetOverlayLayers())
				{
					overlayLayer->OnUpdate(timeStep);
					overlayLayer->OnRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate(timeStep);
		}
	}

	void Application::SetAppIcon(const std::string& imagePath)
	{
		m_Window->SetAppIcon(imagePath);
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>([&](WindowCloseEvent& e)
			{
				Shutdown();
				return false;
			});

		for (OverlayLayer* overlayLayer : m_LayerStack->GetOverlayLayers())
			overlayLayer->OnEvent(event);

		for (Layer* layer : m_LayerStack->GetLayers())
			layer->OnEvent(event);

		m_Scene->OnEvent(event);
	}

	void Application::PushLayer(Layer* layer)
	{
		layer->m_RenderContext = m_Window->GetContext();
		layer->m_Scene = m_Scene;
		m_LayerStack->PushLayer(layer);
	}

	void Application::PushLayer(OverlayLayer* overlayLayer)
	{
		overlayLayer->m_RenderContext = m_Window->GetContext();
		overlayLayer->m_Scene = m_Scene;
		m_LayerStack->PushLayer(overlayLayer);
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack->PopLayer(layer);
	}

	void Application::PopLayer(OverlayLayer* overlayLayer)
	{
		m_LayerStack->PopLayer(overlayLayer);
	}

	void Application::PopLayer(const std::string& layerName)
	{
		m_LayerStack->PopLayer(layerName);
	}

	void Application::PopOverlayLayer(const std::string& overlayLayerName)
	{
		m_LayerStack->PopOverlayLayer(overlayLayerName);
	}

	void Application::PopLastLayer()
	{
		m_LayerStack->PopLastLayer();
	}

	void Application::PopLastOverlayLayer()
	{
		m_LayerStack->PopLastOverlayLayer();
	}

	const Layer* Application::GetLayer(const std::string& layerName) const
	{
		return m_LayerStack->GetLayer(layerName);
	}

	const OverlayLayer* Application::GetOverlayLayer(const std::string& overlayLayerName) const
	{
		return m_LayerStack->GetOverlayLayer(overlayLayerName);
	}

	void Application::SetScene(const Ref<Scene>& scene)
	{
		m_Scene = scene;
	}

	const Ref<Scene>& Application::GetScene() const
	{
		return m_Scene;
	}

	const Ref<Window>& Application::GetWindow() const
	{
		return m_Window;
	}

	Application* Application::Get()
	{
		return m_Instance;
	}
}
