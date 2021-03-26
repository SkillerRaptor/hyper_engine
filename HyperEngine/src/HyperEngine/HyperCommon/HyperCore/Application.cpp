#include "Application.hpp"

#include "Log.hpp"

#include <HyperEvent/EventManager.hpp>
#include <HyperEvent/WindowEvents.hpp>
#include <HyperRendering/Window.hpp>
#include <HyperEngine.hpp>

namespace HyperEngine
{
	void Application::Initialize()
	{
		Log::Init();
		
		m_pEventManager = new EventManager{};
		
		ApplicationInfo applicationInfo{};
		applicationInfo.szTitle = "HyperEngine Application";
		applicationInfo.width = 1280;
		applicationInfo.height = 720;
		applicationInfo.szAppIcon = "assets/textures/AppIcon.png";
		
		OnInitialize();
		
		WindowInfo windowInfo{};
		windowInfo.title = applicationInfo.szTitle;
		windowInfo.width = applicationInfo.width;
		windowInfo.height = applicationInfo.height;
		windowInfo.isDecorated = true;
		windowInfo.isResizable = true;
		windowInfo.isVsync = false;
		windowInfo.isShown = true;
		windowInfo.isFocused = true;
		windowInfo.pEventManager = m_pEventManager;
		
		m_pWindow = new Window{ windowInfo };
		
		m_running = true;
		
		m_pEventManager->RegisterEventListener<WindowCloseEvent>("HyperEngineAppCloseEvent", [&](const WindowCloseEvent& windowCloseEvent)
		{
			m_running = false;
		});
	}
	
	void Application::Terminate()
	{
		OnTerminate();
		
		delete m_pEventManager;
	}
	
	void Application::Run()
	{
		Initialize();
		
		float lastFrameTime{ 0.0f };
		float deltaAccumulator{ 1.0f };
		
		uint32_t framesPerSecond{ 0 };
		uint32_t frames{ 0 };
		
		while (m_running)
		{
			float currentFrameTime = static_cast<float>(m_pWindow->GetTime());
			float deltaFrameTime = static_cast<float>(currentFrameTime - lastFrameTime);
			lastFrameTime = currentFrameTime;
			
			deltaAccumulator -= deltaFrameTime;
			if (deltaAccumulator < 0.0f)
			{
				framesPerSecond = frames;
				frames = 0;
				deltaAccumulator = 1.0f;
			}
			
			OnUpdate(deltaFrameTime);
			OnLateUpdate(deltaFrameTime);
			
			OnRender();
			
			m_pWindow->Present();
			
			frames++;
		}
		
		Terminate();
	}
}