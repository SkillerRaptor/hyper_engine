#include <Engine/Application.hpp>

namespace Engine
{
	
	void Application::InternalInitialize()
	{
		Platform::WindowCreateInfo createInfo{};
		createInfo.title = "Title";
		createInfo.width = 1280;
		createInfo.height = 720;
		createInfo.graphicsApi = Platform::GraphicsApi::OpenGL33;
		m_window = Platform::Window::Construct(createInfo);
		
		Initialize();
		
		m_running = true;
	}
	
	void Application::InternalTerminate()
	{
		Terminate();
		
		m_window->Shutdown();
		delete m_window;
	}
	
	void Application::Run()
	{
		InternalInitialize();
		
		while (m_running)
		{
			m_window->Update();
		}
		
		InternalTerminate();
	}
}
