#include <platform/linux/window.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <core/logger.hpp>

#include <stdio.h>
#include <stdlib.h>

namespace platform::linux
{
	window::window(const window_create_info& create_info)
	{
		m_title = create_info.title;
		m_width = create_info.width;
		m_height = create_info.height;
	}
	
	bool window::initialize()
	{
		m_display = XOpenDisplay(nullptr);
		if (m_display == nullptr)
		{
			core::logger::fatal("Failed to open X display!");
			return false;
		}
		
		m_screen = DefaultScreen(m_display);
		m_window = XCreateSimpleWindow(
			m_display, RootWindow(m_display, m_screen),
			0, 0, m_width, m_height,
			0, BlackPixel(m_display, m_screen), BlackPixel(m_display, m_screen));
		
		XSelectInput(m_display, m_window, ExposureMask | KeyPressMask);
		XMapWindow(m_display, m_window);
		
		return true;
	}
	
	void window::shutdown()
	{
		XCloseDisplay(m_display);
	}
	
	void window::update()
	{
		XEvent event{};
		XNextEvent(m_display, &event);
		
		// TODO: Handle Events
	}
}
#endif
