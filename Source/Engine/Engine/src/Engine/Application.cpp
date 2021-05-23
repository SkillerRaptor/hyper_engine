#include <Engine/Application.hpp>

namespace Engine
{
	void Application::internal_initialize()
	{
		initialize();
	}
	
	void Application::internal_terminate()
	{
		terminate();
	}
	
	void Application::run()
	{
		internal_initialize();
		
		while (m_running)
		{
		
		}
		
		internal_terminate();
	}
}
