/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>
#include <cstdint>
#include <string>

#if HYPERENGINE_PLATFORM_WINDOWS
#	include <Windows.h>
#elif HYPERENGINE_PLATFORM_LINUX
#	include <X11/Xlib.h>
#endif

namespace HyperCore
{
	class CEventManager;
}

namespace HyperPlatform
{
	struct SWindowCreateInfo
	{
		std::string title;
		size_t width;
		size_t height;
		HyperCore::CEventManager* event_manager;
	};
	
	class CWindow
	{
	public:
		~CWindow() = default;
		
		bool initialize(const SWindowCreateInfo& create_info);
		void shutdown();
		
		void poll_events();
		
		void set_title(const std::string& title);
		std::string title() const;
		
		void set_resizable(bool resizeable);
		bool resizable() const;
		
		void set_visible(bool visible);
		bool visible() const;
		
		void set_decorated(bool decorated);
		bool decorated() const;
		
		void set_focused(bool focused);
		bool focused() const;
		
		void set_position(size_t x, size_t y);
		void position(size_t& x, size_t& y) const;
		
		void set_x(size_t x);
		size_t x() const;
		
		void set_y(size_t y);
		size_t y() const;
		
		void set_size(size_t width, size_t height);
		void size(size_t& width, size_t& height) const;
		
		void set_width(size_t width);
		size_t width() const;
		
		void set_height(size_t height);
		size_t height() const;
		
		HyperCore::CEventManager* event_manager() const;

#if HYPERENGINE_PLATFORM_WINDOWS
		HINSTANCE instance() const;
		HWND window() const;
		HDC handle() const;
#elif HYPERENGINE_PLATFORM_LINUX
		Window window() const;
		Display* display() const;
		int32_t screen() const;
#endif
	
	private:
#if HYPERENGINE_PLATFORM_WINDOWS
		DWORD get_window_style() const;
#elif HYPERENGINE_PLATFORM_LINUX
		void update_normal_hints(size_t width, size_t height) const;
#endif
	
	private:
		std::string m_title{ "unknown" };
		bool m_resizable{ true };
		bool m_visible{ true };
		bool m_decorated{ true };
		bool m_focused{ true };
		
		HyperCore::CEventManager* m_event_manager{ nullptr };

#if HYPERENGINE_PLATFORM_WINDOWS
		HINSTANCE m_instance{ nullptr };
		HWND m_window{ nullptr };
		HDC m_handle{ nullptr };
#elif HYPERENGINE_PLATFORM_LINUX
		Window m_window{ 0 };
		Window m_root_window{ 0 };
		Display* m_display{ nullptr };
		int32_t m_screen{ 0 };
#endif
	};
} // namespace HyperPlatform
