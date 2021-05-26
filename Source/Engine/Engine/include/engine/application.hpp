#pragma once

#include <platform/library_manager.hpp>
#include <platform/window.hpp>

namespace engine
{
	class application
	{
	public:
		void run();
	
	protected:
		virtual void initialize() = 0;
		virtual void terminate() = 0;
		
	private:
		void internal_initialize();
		void internal_terminate();
	
	private:
		bool m_running{ false };
		
		platform::window* m_window;
		platform::library_manager* m_library_manager;
	};
}
