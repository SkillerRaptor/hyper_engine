/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/IWindow.hpp>
#include <Windows.h>

namespace HyperPlatform::Windows
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

		HINSTANCE instance() const;
		HWND window() const;
		HDC handle() const;

	private:
		DWORD get_window_style() const;

	private:
		HINSTANCE m_instance{ nullptr };
		HWND m_window{ nullptr };
		HDC m_handle{ nullptr };
	};
} // namespace HyperPlatform::Windows
