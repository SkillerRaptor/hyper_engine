#include <HyperEngine/Application.hpp>

#include <HyperCore/Log.hpp>

namespace HyperEngine
{
	Application::Application()
		: m_running{ false }
	{
	}
	
	void Application::Initialize()
	{
		HyperCore::Log::Initialize();
		
		OnInitialize();
		
		m_running = true;
	}
	
	void Application::Terminate()
	{
		OnTerminate();
		
		HyperCore::Log::Terminate();
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