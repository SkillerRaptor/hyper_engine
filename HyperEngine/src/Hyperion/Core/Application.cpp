#include "Application.hpp"

#include <chrono>

#include "Events/WindowEvents.hpp"
#include "Rendering/Renderer2D.hpp"
#include "Rendering/Buffers/FrameBuffer.hpp"
#include "Utilities/Random.hpp"
#include "Utilities/Timestep.hpp"

#include "Platform/OpenGL/OpenGLWindow.hpp"

namespace Hyperion 
{
	Application* Application::m_Instance;

	Application::Application()
	{
		m_Instance = this;

		m_Window = new OpenGLWindow("HyperEngine", 1920, 1080, false, &m_EventBus);
		m_Window->InitWindow();

		m_Scene = CreateRef<Scene>("Main Scene");

		m_LayerStack = new LayerStack(m_Scene);
		m_LayerStack->GetImGuiLayer()->SetStartX(&m_StartX);
		m_LayerStack->GetImGuiLayer()->SetStartY(&m_StartY);
		m_LayerStack->GetImGuiLayer()->SetSizeX(&m_SizeX);
		m_LayerStack->GetImGuiLayer()->SetSizeY(&m_SizeY);

		Renderer2D::SetShaderManager(m_Window->GetContext()->GetShaderManager());
		Renderer2D::SetTextureManager(m_Window->GetContext()->GetTextureManager());
		Renderer2D::Init();

		Random::Init();
	}

	void Application::Shutdown()
	{
		m_Running = false;

		for (Layer* layer : m_LayerStack->GetLayers())
		{
			layer->OnDetach();
			m_LayerStack->PopLayer();
		}

		m_Window->ShutdownWindow();

		delete m_LayerStack;
		delete m_Window;
	}

	void Application::PushLayer(Layer* layer)
	{
		layer->SetRenderContext(m_Window->GetContext());
		m_LayerStack->PushLayer(layer);
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack->PopLayer(layer);
	}

	void Application::Run()
	{
		const int ticksPerSecond = 60;
		const int ticksPerFrame = 5;
		const int skipTicks = 1000 / ticksPerSecond;
		int loops = 0;
		int currentTick = 0;

		std::chrono::time_point currentTime = std::chrono::system_clock::now();

		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

		TextureManager* textureManager = m_Window->GetContext()->GetTextureManager();
		uint32_t bufferTexture = textureManager->CreateTexture(m_Window->GetWindowData().Width, m_Window->GetWindowData().Height, TextureType::FRAMEBUFFER);
		uint32_t bufferTextureId = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(bufferTexture))->TextureId;

		m_LayerStack->GetImGuiLayer()->SetFrameTextureId(bufferTextureId);

		FrameBuffer frameBuffer;

		while (m_Running)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			OpenGLTextureData* textureData = static_cast<OpenGLTextureData*>(textureManager->GetTextureData(bufferTexture));
			textureData->Width = m_Window->GetWindowData().Width;
			textureData->Height = m_Window->GetWindowData().Height;
			textureManager->DeleteTextureData(bufferTexture);
			textureManager->GenerateTexture(textureData, true);

			frameBuffer.Bind();
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, bufferTextureId, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			frameBuffer.Unbind();

			loops = 0;
			while (std::chrono::system_clock::now() > currentTime && loops < ticksPerFrame)
			{
				for (Layer* layer : m_LayerStack->GetLayers())
					layer->OnTick(currentTick);

				m_Scene->OnTick(currentTick);

				currentTick++;
				if (currentTick >= ticksPerSecond)
					currentTick = 0;

				currentTime += std::chrono::milliseconds(skipTicks);
				loops++;
			}

			const long long currentTimeNow = std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime).time_since_epoch().count();
			const long long timeSinceEpoch = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

			const Timestep timeStep = static_cast<double>(timeSinceEpoch + skipTicks - currentTimeNow) / static_cast<double>(skipTicks);

			if (!m_Running) break;

			while (m_EventBus.size() > 0)
			{
				Ref<Event> e = m_EventBus.front();
				if (e->Handled) continue;
				OnEvent(*e);
				m_EventBus.pop();
			}

			if (!m_Running) break;

			frameBuffer.Bind();

			for (Layer* layer : m_LayerStack->GetLayers())
			{
				layer->OnUpdate(timeStep);
				layer->OnRender();
			}

			m_Scene->OnUpdate(timeStep);
			m_Scene->OnRender();

			frameBuffer.Unbind();

			if (!m_Running) break;

			m_LayerStack->GetImGuiLayer()->OnUpdate(timeStep);
			m_LayerStack->GetImGuiLayer()->OnRender();

			m_Window->OnUpdate(timeStep);
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>([&](WindowCloseEvent& e)
			{
				Shutdown();
				return false;
			});

		for (Layer* layer : m_LayerStack->GetLayers())
			layer->OnEvent(event);

		m_Scene->OnEvent(event);
	}

	Window* Application::GetNativeWindow() const
	{
		return m_Window;
	}

	Application* Application::Get()
	{
		return m_Instance;
	}
}
