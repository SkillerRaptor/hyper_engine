#include "Application.hpp"

namespace Engine
{
	void Application::initialize()
	{
		platform::window_create_info create_info{};
		create_info.title = "Title";
		create_info.width = 1280;
		create_info.height = 720;
		create_info.api = platform::graphics_api::vulkan;
		
		m_library_manager = platform::library_manager::construct();
		
		m_window = platform::window::construct(create_info);
		m_window->initialize(m_library_manager);
		
		startup();
		
		m_running = true;
	}
	
	void Application::terminate()
	{
		shutdown();
		
		m_window->shutdown();
		delete m_window;
		
		delete m_library_manager;
	}
	
	void Application::run()
	{
		initialize();
		
		while (m_running)
		{
			m_window->update();
		}
		
		terminate();
	}
}
