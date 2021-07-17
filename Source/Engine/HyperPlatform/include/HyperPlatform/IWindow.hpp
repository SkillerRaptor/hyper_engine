/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <string>

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
	
	class IWindow
	{
	public:
		virtual ~IWindow() = default;
		
		virtual bool initialize(const SWindowCreateInfo& create_info) = 0;
		virtual void shutdown() = 0;
		
		virtual void poll_events() = 0;
		
		virtual void set_title(const std::string& title) = 0;
		virtual std::string title() const = 0;
		
		virtual void set_resizable(bool resizeable) = 0;
		virtual bool resizable() const = 0;
		
		virtual void set_visible(bool visible) = 0;
		virtual bool visible() const = 0;
		
		virtual void set_decorated(bool decorated) = 0;
		virtual bool decorated() const = 0;
		
		virtual void set_focused(bool focused) = 0;
		virtual bool focused() const = 0;
		
		virtual void set_position(size_t x, size_t y) = 0;
		virtual void position(size_t& x, size_t& y) const = 0;
		
		virtual void set_x(size_t x) = 0;
		virtual size_t x() const = 0;
		
		virtual void set_y(size_t y) = 0;
		virtual size_t y() const = 0;
		
		virtual void set_size(size_t width, size_t height) = 0;
		virtual void size(size_t& width, size_t& height) const = 0;
		
		virtual void set_width(size_t width) = 0;
		virtual size_t width() const = 0;
		
		virtual void set_height(size_t height) = 0;
		virtual size_t height() const = 0;
		
		virtual HyperCore::CEventManager* event_manager() const = 0;
		
		static IWindow* construct();
	
	protected:
		std::string m_title{ "unknown" };
		bool m_resizable{ true };
		bool m_visible{ true };
		bool m_decorated{ true };
		bool m_focused{ true };
		
		HyperCore::CEventManager* m_event_manager{ nullptr };
	};
} // namespace HyperPlatform
