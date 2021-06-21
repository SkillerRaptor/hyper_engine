/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Events/WindowEvents.hpp>

namespace HyperCore
{
	CWindowResizeEvent::CWindowResizeEvent(uint32_t width, uint32_t height)
		: m_width(width)
		, m_height(height)
	{
	}

	uint32_t CWindowResizeEvent::width() const
	{
		return m_width;
	}

	uint32_t CWindowResizeEvent::height() const
	{
		return m_height;
	}

	std::string CWindowResizeEvent::name() const
	{
		return "WindowResize";
	}

	IEvent::EType CWindowResizeEvent::type() const
	{
		return IEvent::EType::WindowResize;
	}

	IEvent::ECategory CWindowResizeEvent::category() const
	{
		return IEvent::Window;
	}

	std::string CWindowCloseEvent::name() const
	{
		return "WindowClose";
	}

	IEvent::EType CWindowCloseEvent::type() const
	{
		return IEvent::EType::WindowClose;
	}

	IEvent::ECategory CWindowCloseEvent::category() const
	{
		return IEvent::Window;
	}

	std::string CWindowFocusEvent::name() const
	{
		return "WindowFocus";
	}

	IEvent::EType CWindowFocusEvent::type() const
	{
		return IEvent::EType::WindowFocus;
	}

	IEvent::ECategory CWindowFocusEvent::category() const
	{
		return IEvent::Window;
	}

	std::string CWindowLostFocusEvent::name() const
	{
		return "WindowLostFocus";
	}

	IEvent::EType CWindowLostFocusEvent::type() const
	{
		return IEvent::EType::WindowLostFocus;
	}

	IEvent::ECategory CWindowLostFocusEvent::category() const
	{
		return IEvent::Window;
	}

	CWindowMovedEvent::CWindowMovedEvent(
		uint32_t position_x,
		uint32_t position_y)
		: m_position_x(position_x)
		, m_position_y(position_y)
	{
	}

	uint32_t CWindowMovedEvent::position_x() const
	{
		return m_position_x;
	}

	uint32_t CWindowMovedEvent::position_y() const
	{
		return m_position_y;
	}

	std::string CWindowMovedEvent::name() const
	{
		return "WindowMoved";
	}

	IEvent::EType CWindowMovedEvent::type() const
	{
		return IEvent::EType::WindowMoved;
	}

	IEvent::ECategory CWindowMovedEvent::category() const
	{
		return IEvent::Window;
	}
} // namespace HyperCore
