#include <engine/application.hpp>

namespace engine
{
	void application::internal_initialize()
	{
		platform::window_create_info create_info{};
		create_info.title = "Title";
		create_info.width = 1280;
		create_info.height = 720;
		create_info.api = platform::graphics_api::opengl33;
		
		m_library_manager = platform::library_manager::construct();
		
		m_window = platform::window::construct(create_info);
		m_window->initialize(m_library_manager);
		
		initialize();
		
		m_running = true;
	}
	
	void application::internal_terminate()
	{
		terminate();
		
		m_window->shutdown();
		delete m_window;
		
		delete m_library_manager;
	}
	
	void application::run()
	{
		internal_initialize();
		
		while (m_running)
		{
			m_window->update();
		}
		
		internal_terminate();
	}
}