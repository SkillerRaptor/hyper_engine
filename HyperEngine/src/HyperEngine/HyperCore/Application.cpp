#include "HyperCore/Application.hpp"

#include "HyperECS/HyperComponents/Components.hpp"
#include "HyperEvent/WindowEvents.hpp"
#include "HyperRendering/ImGuiLayer.hpp"
#include "HyperUtilities/Timestep.hpp"

namespace HyperCore
{
	Application* Application::m_Instance;

	Application::Application(const std::string& title, uint32_t width, uint32_t height)
	{
		Init(title, width, height);
	}

	void Application::Init(const std::string& title, uint32_t width, uint32_t height)
	{
		m_Instance = this;

		HP_CORE_INFO("");
		HP_CORE_INFO("_     _ __   __  _____  _______  ______ _______ __   _  ______ _____ __   _ _______");
		HP_CORE_INFO("|_____|   \\_/   |_____] |______ |_____/ |______ | \\  | |  ____   |   | \\  | |______");
		HP_CORE_INFO("|     |    |    |       |______ |    \\_ |______ |  \\_| |_____| __|__ |  \\_| |______");
		HP_CORE_INFO("");

		HyperRendering::WindowPropsInfo windowPropsInfo{};
		windowPropsInfo.Title = title;
		windowPropsInfo.Width = width;
		windowPropsInfo.Height = height;
		windowPropsInfo.EventBus = &m_EventBus;

		m_Window = HyperRendering::Window::Construct(windowPropsInfo);

		m_Scene = CreateRef<HyperRendering::Scene>("Main Scene", m_Window->GetContext()->GetRenderer2D());

		m_LayerStack = CreateScope<HyperLayer::LayerStack>();
		m_ImGuiLayer = new HyperRendering::ImGuiLayer();
		PushLayer(m_ImGuiLayer);
	}

	void Application::Shutdown()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		m_Window->GetContext()->GetRenderer2D()->Setup();

		double lastTime = 0.0;

		Ref<HyperRendering::SceneRecorder> sceneRecorder = m_Window->GetContext()->GetSceneRecorder();
		bool enableCapture = m_LayerStack->GetOverlayLayers().size() > 1;

		if (enableCapture)
			sceneRecorder->InitRecorder();

		while (m_Running)
		{
			double currentTime = m_Window->GetTime();
			const HyperUtilities::Timestep timeStep = currentTime - lastTime;
			lastTime = currentTime;

			m_Window->OnPreUpdate();

			while (!m_EventBus.empty())
			{
				Ref<HyperEvent::Event> e = m_EventBus.front();
				if (e->Handled) continue;
				OnEvent(*e);
				m_EventBus.pop();
			}

			bool validCamera = false;
			HyperECS::TransformComponent transformComponent;
			HyperECS::CameraComponent cameraComponent;

			for (HyperECS::Entity entity : m_Scene->GetRegistry().Each<HyperECS::CameraComponent>())
			{
				HyperECS::CameraComponent& entityCameraComponent = m_Scene->GetRegistry().GetComponent<HyperECS::CameraComponent>(entity);
				if (!entityCameraComponent.IsPrimary())
					continue;

				validCamera = true;
				transformComponent = m_Scene->GetRegistry().GetComponent<HyperECS::TransformComponent>(entity);
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

				for (HyperLayer::Layer* layer : m_LayerStack->GetLayers())
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

				HyperRendering::EditorCamera& editorCamera = m_ImGuiLayer->GetEditorCamera();
				m_Window->GetContext()->GetRenderer2D()->SetCamera(editorCamera.GetProjectionMatrix(), editorCamera.GetViewMatrix());

				sceneRecorder->StartEditorRecording();

				for (HyperLayer::Layer* layer : m_LayerStack->GetLayers())
				{
					layer->OnUpdate(timeStep);
					layer->OnRender();
				}

				m_Scene->OnUpdate(timeStep);
				m_Scene->OnRender();

				sceneRecorder->EndEditorRecording();

				m_ImGuiLayer->Start();
				for (HyperLayer::OverlayLayer* overlayLayer : m_LayerStack->GetOverlayLayers())
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

	void Application::OnEvent(HyperEvent::Event& event)
	{
		HyperEvent::EventDispatcher dispatcher(event);

		dispatcher.Dispatch<HyperEvent::WindowCloseEvent>([&](HyperEvent::WindowCloseEvent& e)
			{
				Shutdown();
				return false;
			});

		for (HyperLayer::OverlayLayer* overlayLayer : m_LayerStack->GetOverlayLayers())
			overlayLayer->OnEvent(event);

		for (HyperLayer::Layer* layer : m_LayerStack->GetLayers())
			layer->OnEvent(event);
	}
}
