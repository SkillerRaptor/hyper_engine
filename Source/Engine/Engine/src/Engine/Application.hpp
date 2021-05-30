#pragma once

#include <platform/library_manager.hpp>
#include <platform/window.hpp>

namespace Engine
{
	class Application
	{
	public:
		void run();
	
	protected:
		virtual void startup() = 0;
		virtual void shutdown() = 0;
		
	private:
		void initialize();
		void terminate();
	
	private:
		bool m_running{ false };
		
		platform::window* m_window;
		platform::library_manager* m_library_manager;
	};
}
