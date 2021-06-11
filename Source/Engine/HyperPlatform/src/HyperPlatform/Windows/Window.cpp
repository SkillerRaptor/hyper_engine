/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <HyperCore/Logger.hpp>
#include <HyperPlatform/Windows/Window.hpp>

namespace HyperPlatform::Windows
{
	LRESULT CALLBACK win_proc(HWND window, UINT message, WPARAM first_parameter, LPARAM second_parameter)
	{
		// TODO: Handle Events
		
		switch (message)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			break;
		default:
			break;
		}
		
		return DefWindowProc(window, message, first_parameter, second_parameter);
	}
	
	bool CWindow::initialize(const SWindowCreateInfo& create_info)
	{
		m_instance = GetModuleHandle(nullptr);
		
		WNDCLASSEX window_class{};
		window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.hInstance = m_instance;
		window_class.lpfnWndProc = win_proc;
		window_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		window_class.hIconSm = window_class.hIcon;
		window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		window_class.lpszMenuName = nullptr;
		window_class.lpszClassName = m_title.c_str();
		window_class.cbSize = sizeof(WNDCLASSEX);
		
		if (!RegisterClassEx(&window_class))
		{
			HyperCore::CLogger::fatal("Failed to register window class! Error: {}", GetLastError());
			return false;
		}
		
		RECT window_rect{};
		window_rect.left = 0;
		window_rect.top = 0;
		window_rect.right = static_cast<LONG>(create_info.width);
		window_rect.bottom = static_cast<LONG>(create_info.height);
		
		AdjustWindowRectEx(&window_rect, get_window_style(), false, 0);
		
		m_handle = CreateWindowEx(
			0, window_class.lpszClassName, m_title.c_str(), get_window_style(), 0, 0,
			window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
			nullptr, nullptr, m_instance, nullptr);
		if (!m_handle)
		{
			HyperCore::CLogger::fatal("Failed to create window! Error: {}", GetLastError());
			return false;
		}
		
		ShowWindow(m_handle, SW_SHOWDEFAULT);
		UpdateWindow(m_handle);
		SetForegroundWindow(m_handle);
		SetFocus(m_handle);
		
		ShowCursor(true);
		
		return true;
	}
	
	void CWindow::shutdown()
	{
		PostQuitMessage(0);
	}
	
	void CWindow::poll_events()
	{
		MSG message;
		
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	
	void CWindow::set_title(const std::string& title)
	{
		m_title = title;
		
		SetWindowText(m_handle, title.c_str());
	}
	
	std::string CWindow::title() const
	{
		return m_title;
	}
	
	void CWindow::set_position(size_t x, size_t y)
	{
		RECT rect{};
		rect.left = static_cast<LONG>(x);
		rect.top = static_cast<LONG>(y);
		rect.right = static_cast<LONG>(x);
		rect.bottom = static_cast<LONG>(y);
		
		AdjustWindowRectEx(&rect, get_window_style(), false, 0);
		
		SetWindowPos(
			m_handle, nullptr,
			rect.left, rect.top, 0, 0,
			SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	
	void CWindow::position(size_t& x, size_t& y) const
	{
		RECT rect{};
		GetWindowRect(m_handle, &rect);
		
		x = rect.left;
		y = rect.top;
	}
	
	void CWindow::set_x(size_t x)
	{
		set_size(x, y());
	}
	
	size_t CWindow::x() const
	{
		size_t x = 0;
		size_t y = 0;
		position(x, y);
		
		return x;
	}
	
	void CWindow::set_y(size_t y)
	{
		set_size(x(), y);
	}
	
	size_t CWindow::y() const
	{
		size_t x = 0;
		size_t y = 0;
		position(x, y);
		
		return y;
	}
	
	void CWindow::set_size(size_t width, size_t height)
	{
		RECT rect{};
		rect.left = 0;
		rect.top = 0;
		rect.right = static_cast<LONG>(width);
		rect.bottom = static_cast<LONG>(height);
		
		AdjustWindowRectEx(&rect, get_window_style(), false, 0);
		
		SetWindowPos(
			m_handle, HWND_TOP,
			0, 0, rect.right - rect.left, rect.bottom - rect.top,
			SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
	}
	
	void CWindow::size(size_t& width, size_t& height) const
	{
		RECT rect{};
		GetWindowRect(m_handle, &rect);
		
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
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
	
	DWORD CWindow::get_window_style() const
	{
		DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX;
		
		if (!m_decorated)
		{
			style |= WS_POPUP;
			return style;
		}
		
		style |= WS_CAPTION;
		
		if (!m_resizable)
		{
			return style;
		}
		
		style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
		
		return style;
	}
	
	HINSTANCE CWindow::instance() const
	{
		return m_instance;
	}
	
	HWND CWindow::handle() const
	{
		return m_handle;
	}
}
#endif
