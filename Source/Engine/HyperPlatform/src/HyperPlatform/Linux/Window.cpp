/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <HyperCore/Logger.hpp>
#include <HyperPlatform/Linux/Window.hpp>
#include <X11/Xutil.h>

namespace HyperPlatform::Linux
{
	bool CWindow::initialize(const SWindowCreateInfo& create_info)
	{
		m_title = create_info.title;
		
		m_display = XOpenDisplay(nullptr);
		if (m_display == nullptr)
		{
			HyperCore::CLogger::fatal("Failed to connect to X display!");
			return false;
		}
		
		m_screen = DefaultScreen(m_display);
		m_window = XCreateSimpleWindow(
			m_display, RootWindow(m_display, m_screen),
			0, 0, create_info.width, create_info.height,
			0, BlackPixel(m_display, m_screen), BlackPixel(m_display, m_screen));
		
		XSelectInput(m_display, m_window, ExposureMask | KeyPressMask);
		XMapWindow(m_display, m_window);
		
		return true;
	}
	
	void CWindow::shutdown()
	{
		XCloseDisplay(m_display);
	}
	
	void CWindow::poll_events()
	{
		// TODO: Handle Events
		
		XEvent event{};
		XNextEvent(m_display, &event);
	}
	
	void CWindow::set_title(const std::string& title)
	{
		m_title = title;
		
		XStoreName(m_display, m_window, title.c_str());
	}
	
	std::string CWindow::title() const
	{
		return m_title;
	}
	
	void CWindow::set_position(size_t x, size_t y)
	{
		XMoveWindow(m_display, m_window, static_cast<int>(x), static_cast<int>(y));
		XFlush(m_display);
	}
	
	void CWindow::position(size_t& x, size_t& y) const
	{
		Window dummy = 0;
		int window_x = 0;
		int window_y = 0;
		
		XTranslateCoordinates(m_display, m_window, m_window, 0, 0, &window_x, &window_y, &dummy);
		
		x = window_x;
		y = window_y;
	}
	
	void CWindow::set_x(size_t x)
	{
		set_size(x, y());
	}
	
	size_t CWindow::x() const
	{
		size_t x{};
		size_t y{};
		position(x, y);
		
		return x;
	}
	
	void CWindow::set_y(size_t y)
	{
		set_size(x(), y);
	}
	
	size_t CWindow::y() const
	{
		size_t x{};
		size_t y{};
		position(x, y);
		
		return y;
	}
	
	void CWindow::set_size(size_t width, size_t height)
	{
		if (!m_resizable)
		{
			update_normal_hints(width, height);
		}
		
		XResizeWindow(m_display, m_window, width, height);
		XFlush(m_display);
	}
	
	void CWindow::size(size_t& width, size_t& height) const
	{
		XWindowAttributes attributes;
		XGetWindowAttributes(m_display, m_window, &attributes);
		
		width = attributes.width;
		height = attributes.height;
	}
	
	void CWindow::set_width(size_t width)
	{
		set_size(width, height());
	}
	
	size_t CWindow::width() const
	{
		size_t width{};
		size_t height{};
		size(width, height);
		
		return width;
	}
	
	void CWindow::set_height(size_t height)
	{
		set_size(width(), height);
	}
	
	size_t CWindow::height() const
	{
		size_t width{};
		size_t height{};
		size(width, height);
		
		return height;
	}
	
	void CWindow::update_normal_hints(size_t width, size_t height) const
	{
		XSizeHints* hints = XAllocSizeHints();
		
		if (!m_resizable)
		{
			hints->flags |= (PMinSize | PMaxSize);
			hints->min_width = static_cast<int>(width);
			hints->max_width = static_cast<int>(width);
			hints->min_height = static_cast<int>(height);
			hints->max_height = static_cast<int>(height);
		}
		
		hints->flags |= PWinGravity;
		hints->win_gravity = StaticGravity;
		
		XSetWMNormalHints(m_display, m_window, hints);
		XFree(hints);
	}
	
	Window CWindow::window() const
	{
		return m_window;
	}
	
	Display* CWindow::display() const
	{
		return m_display;
	}
	
	int32_t CWindow::screen() const
	{
		return m_screen;
	}
}
#endif
