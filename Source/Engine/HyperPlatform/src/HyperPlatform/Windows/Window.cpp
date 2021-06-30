/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Logger.hpp>
#include <HyperCore/Events/EventManager.hpp>
#include <HyperCore/Events/WindowEvents.hpp>
#include <HyperCore/Utilities/Prerequisites.hpp>
#include <HyperPlatform/Windows/Window.hpp>

namespace HyperPlatform::Windows
{
	LRESULT CALLBACK win_proc(
		HWND h_window,
		UINT message,
		WPARAM first_parameter,
		LPARAM second_parameter)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			LPCREATESTRUCT create_struct =
				reinterpret_cast<LPCREATESTRUCT>(second_parameter);
			CWindow* window =
				reinterpret_cast<CWindow*>(create_struct->lpCreateParams);

			SetWindowLongPtr(
				h_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			return 0;
		}
		case WM_CLOSE:
		{
			CWindow* window = reinterpret_cast<CWindow*>(
				GetWindowLongPtr(h_window, GWLP_USERDATA));

			window->event_manager()->invoke<HyperCore::SWindowCloseEvent>();

			return 0;
		}
		default:
			break;
		}

		return DefWindowProc(
			h_window, message, first_parameter, second_parameter);
	}

	bool CWindow::initialize(const SWindowCreateInfo& create_info)
	{
		m_title = create_info.title;
		m_event_manager = create_info.event_manager;

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
		window_class.hbrBackground =
			static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		window_class.lpszMenuName = nullptr;
		window_class.lpszClassName = m_title.c_str();
		window_class.cbSize = sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&window_class))
		{
			HyperCore::CLogger::fatal(
				"Failed to register window class! Error: {}", GetLastError());
			return false;
		}

		RECT window_rect{};
		window_rect.left = 0;
		window_rect.top = 0;
		window_rect.right = static_cast<LONG>(create_info.width);
		window_rect.bottom = static_cast<LONG>(create_info.height);

		AdjustWindowRectEx(&window_rect, get_window_style(), false, 0);

		m_window = CreateWindowEx(
			0,
			window_class.lpszClassName,
			m_title.c_str(),
			get_window_style(),
			0,
			0,
			window_rect.right - window_rect.left,
			window_rect.bottom - window_rect.top,
			nullptr,
			nullptr,
			m_instance,
			this);
		if (!m_window)
		{
			HyperCore::CLogger::fatal(
				"Failed to create window! Error: {}", GetLastError());
			return false;
		}

		m_handle = GetDC(m_window);

		ShowWindow(m_window, SW_SHOWDEFAULT);
		UpdateWindow(m_window);
		SetForegroundWindow(m_window);
		SetFocus(m_window);

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

		SetWindowText(m_window, title.c_str());
	}

	std::string CWindow::title() const
	{
		return m_title;
	}

	void CWindow::set_resizable(bool)
	{
		// TODO: Implement set_resizable(bool resizeable)
	}

	bool CWindow::resizable() const
	{
		return m_resizable;
	}

	void CWindow::set_visible(bool)
	{
		// TODO: Implement set_visible(bool visible)
	}

	bool CWindow::visible() const
	{
		return m_visible;
	}

	void CWindow::set_decorated(bool)
	{
		// TODO: Implement set_decorated(bool decorated)
	}

	bool CWindow::decorated() const
	{
		return m_decorated;
	}

	void CWindow::set_focused(bool)
	{
		// TODO: Implement set_focused(bool focused)
	}

	bool CWindow::focused() const
	{
		return m_focused;
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
			m_window,
			nullptr,
			rect.left,
			rect.top,
			0,
			0,
			SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	void CWindow::position(size_t& x, size_t& y) const
	{
		RECT rect{};
		GetWindowRect(m_window, &rect);

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
			m_window,
			HWND_TOP,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
	}

	void CWindow::size(size_t& width, size_t& height) const
	{
		RECT rect{};
		GetWindowRect(m_window, &rect);

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
		DWORD style =
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX;

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

	HyperCore::CEventManager* CWindow::event_manager() const
	{
		return m_event_manager;
	}

	HINSTANCE CWindow::instance() const
	{
		return m_instance;
	}

	HWND CWindow::window() const
	{
		return m_window;
	}

	HDC CWindow::handle() const
	{
		return m_handle;
	}
} // namespace HyperPlatform::Windows
