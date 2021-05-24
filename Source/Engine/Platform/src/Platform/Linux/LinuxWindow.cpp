#include <Platform/Linux/LinuxWindow.hpp>

#if HYPERENGINE_PLATFORM_LINUX
	
#include <Core/Logger.hpp>

#include <stdio.h>
#include <stdlib.h>

namespace Platform
{
	LinuxWindow::LinuxWindow(const WindowCreateInfo& createInfo)
	{
		m_title = createInfo.title;
		m_width = createInfo.width;
		m_height = createInfo.height;
		
		m_display = XOpenDisplay(nullptr);
		if (m_display == nullptr)
		{
			Core::Logger::Fatal("Failed to open X display!");
			std::exit(1);
		}
		
		m_screen = DefaultScreen(m_display);
		m_window = XCreateSimpleWindow(
			m_display, RootWindow(m_display, m_screen),
			0, 0, m_width, m_height,
			0, BlackPixel(m_display, m_screen), BlackPixel(m_display, m_screen));
		
		XSelectInput(m_display, m_window, ExposureMask | KeyPressMask);
		XMapWindow(m_display, m_window);
	}
	
	void LinuxWindow::Shutdown()
	{
		XCloseDisplay(m_display);
	}
	
	void LinuxWindow::Update()
	{
		XEvent event{};
		XNextEvent(m_display, &event);
	}
}
#endif
