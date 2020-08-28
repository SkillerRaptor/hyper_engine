#include "Application.h"

namespace Hyperion {

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running);
	}

	void Application::Shutdown()
	{
		m_Running = false;
	}
}