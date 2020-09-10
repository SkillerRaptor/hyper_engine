#include "Application.hpp"

#include <chrono>

#include "Events/WindowEvents.hpp"
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

		/* Currently only OpenGL */
		m_Window = new OpenGLWindow("HyperEngine", 1280, 720, false, &m_EventBus);
		m_Window->InitWindow();

		m_LayerStack = new LayerStack();
		
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

		uint32_t handle = m_Window->GetContext()->GetTextureManager()->CreateTexture(m_Window->GetWindowData().Width, m_Window->GetWindowData().Height, TextureType::FRAMEBUFFER);
		//Texture frameTexture;
		while (m_Running)
		{
			//m_Window->GetContext()->GetTextureManager()->Set(m_Window->GetWindowData().Width, m_Window->GetWindowData().Height);
			//frameTexture.SetWidth(m_Window->GetWindowData().Width);
			//frameTexture.SetHeight(m_Window->GetWindowData().Height);
			//frameTexture.GenerateTexture(nullptr, true);

			FrameBuffer frameBuffer;
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_cast<OpenGLTextureData*>(m_Window->GetContext()->GetTextureManager()->GetTextureData(handle))->TextureId, 0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			loops = 0;
			while (std::chrono::system_clock::now() > currentTime && loops < ticksPerFrame)
			{
				for (Layer* layer : m_LayerStack->GetLayers())
					layer->OnTick(currentTick);

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
				std::shared_ptr<Event> e = m_EventBus.front();
				if (e->Handled) continue;
				OnEvent(*e);
				m_EventBus.pop();
			}

			if (!m_Running) break;

			for (Layer* layer : m_LayerStack->GetLayers())
			{
				layer->OnUpdate(timeStep);
				layer->OnRender();
			}

			frameBuffer.Unbind();

			if (!m_Running) break;

			m_LayerStack->GetImGuiLayer()->OnUpdate(timeStep);
			m_LayerStack->GetImGuiLayer()->OnRender(static_cast<OpenGLTextureData*>(m_Window->GetContext()->GetTextureManager()->GetTextureData(handle))->TextureId);

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