#pragma once

#include "hppch.h"

namespace Hyperion {

	class Application
	{
	private:
		bool m_Running = true;

	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		void Shutdown();
	};

	Application* CreateApplication();
}