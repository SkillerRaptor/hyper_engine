#pragma once

#include <Platform/Window.hpp>

namespace Engine
{
	class Application
	{
	public:
		void Run();
	
	protected:
		virtual void Initialize() = 0;
		virtual void Terminate() = 0;
		
	private:
		void InternalInitialize();
		void InternalTerminate();
	
	private:
		bool m_running{ false };
		
		Platform::Window* m_window;
	};
}
