#pragma once

namespace Engine
{
	class Application
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
	};
}
