#include <HyperEngine/Application.hpp>

#include <HyperCore/Logger.hpp>

namespace HyperEngine
{
	Application::Application()
		: m_running{ false }
	{
	}
	
	void Application::Initialize()
	{
		
		OnInitialize();
		
		m_running = true;
	}
	
	void Application::Terminate()
	{
		OnTerminate();
	}
	
	void Application::Run()
	{
		Initialize();
		
		while (m_running)
		{
			float deltaTime{ 0.0f };
			
			OnUpdate(deltaTime);
			OnLateUpdate(deltaTime);
			
			OnRender();
		}
		
		Terminate();
	}
}
