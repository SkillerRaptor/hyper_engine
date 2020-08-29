#include "Application.h"

namespace Hyperion {

	Application::Application()
	{
		m_Window = new Window("HyperEngine", 1280, 720, false);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		const int ticksPerSecond = 60;
		const int ticksPerFrame = 5;
		const int skipTicks = 1000 / ticksPerSecond;
		int loops = 0;
		int currentTick = 0;

		std::chrono::time_point currentTime = std::chrono::system_clock::now();

		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		while (m_Running)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			loops = 0;
			while (std::chrono::system_clock::now() > currentTime && loops < ticksPerFrame)
			{
				/* Layer Tick */

				currentTick++;
				if (currentTick >= ticksPerSecond)
					currentTick = 0;

				currentTime += std::chrono::milliseconds(skipTicks);
				loops++;
			}

			const long long currentTimeNow = std::chrono::time_point_cast<std::chrono::milliseconds>(currentTime).time_since_epoch().count();
			const long long timeSinceEpoch = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

			const float interpolation = static_cast<float>(timeSinceEpoch + skipTicks - currentTimeNow) / static_cast<float>(skipTicks);

			if (!m_Running) break;

			/* Events */

			/* Layer Update & Render */

			m_Window->OnUpdate();
		}
	}

	void Application::Shutdown()
	{
		m_Running = false;
		delete m_Window;
	}
}