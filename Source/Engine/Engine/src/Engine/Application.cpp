#include <Engine/Application.hpp>

namespace Engine
{
	void Application::InternalInitialize()
	{
		Platform::WindowCreateInfo createInfo{};
		createInfo.title = "";
		createInfo.width = 1280;
		createInfo.height = 720;
		m_window = Platform::Window::Construct(createInfo);
	
		Initialize();
	}
	
	void Application::InternalTerminate()
	{
		Terminate();
		
		delete m_window;
	}
	
	void Application::Run()
	{
		InternalInitialize();
		
		while (m_running)
		{
		
		}
		
		InternalTerminate();
	}
}
