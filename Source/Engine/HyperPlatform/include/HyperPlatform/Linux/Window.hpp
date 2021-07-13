/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/IWindow.hpp>
#include <X11/Xlib.h>

namespace HyperPlatform::Linux
{
	class CWindow final : public IWindow
	{
	public:
		CWindow() = default;
		virtual ~CWindow() override = default;

		virtual bool initialize(const SWindowCreateInfo& create_info) override;
		virtual void shutdown() override;

		virtual void poll_events() override;

		virtual void set_title(const std::string& title) override;
		virtual std::string title() const override;

		virtual void set_resizable(bool resizeable) override;
		virtual bool resizable() const override;

		virtual void set_visible(bool visible) override;
		virtual bool visible() const override;

		virtual void set_decorated(bool decorated) override;
		virtual bool decorated() const override;

		virtual void set_focused(bool focused) override;
		virtual bool focused() const override;

		virtual void set_position(size_t x, size_t y) override;
		virtual void position(size_t& x, size_t& y) const override;

		virtual void set_x(size_t x) override;
		virtual size_t x() const override;

		virtual void set_y(size_t y) override;
		virtual size_t y() const override;

		virtual void set_size(size_t width, size_t height) override;
		virtual void size(size_t& width, size_t& height) const override;

		virtual void set_width(size_t width) override;
		virtual size_t width() const override;

		virtual void set_height(size_t height) override;
		virtual size_t height() const override;

		virtual HyperCore::CEventManager* event_manager() const override;

		Window window() const;
		Display* display() const;
		int32_t screen() const;

	private:
		void update_normal_hints(size_t width, size_t height) const;

	private:
		Window m_window{ 0 };
		Window m_root_window{ 0 };
		Display* m_display{ nullptr };
		int32_t m_screen{ 0 };
	};
} // namespace HyperPlatform::Linux
