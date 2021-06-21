/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Events/MouseEvents.hpp>

namespace HyperCore
{
	CMouseMovedEvent::CMouseMovedEvent(float position_x, float position_y)
		: m_position_x(position_x)
		, m_position_y(position_y)
	{
	}

	float CMouseMovedEvent::position_x() const
	{
		return m_position_x;
	}

	float CMouseMovedEvent::position_y() const
	{
		return m_position_y;
	}

	std::string CMouseMovedEvent::name() const
	{
		return "MouseMoved";
	}

	IEvent::EType CMouseMovedEvent::type() const
	{
		return IEvent::EType::MouseMoved;
	}

	IEvent::ECategory CMouseMovedEvent::category() const
	{
		return IEvent::ECategory::Mouse | IEvent::ECategory::Input;
	}

	CMouseScrolledEvent::CMouseScrolledEvent(float offset_x, float offset_y)
		: m_offset_x(offset_x)
		, m_offset_y(offset_y)
	{
	}

	float CMouseScrolledEvent::offset_x() const
	{
		return m_offset_x;
	}

	float CMouseScrolledEvent::offset_y() const
	{
		return m_offset_y;
	}

	std::string CMouseScrolledEvent::name() const
	{
		return "MouseScrolled";
	}

	IEvent::EType CMouseScrolledEvent::type() const
	{
		return IEvent::EType::MouseScrolled;
	}

	IEvent::ECategory CMouseScrolledEvent::category() const
	{
		return IEvent::ECategory::Mouse | IEvent::ECategory::Input;
	}

	IMouseButtonEvent::IMouseButtonEvent(int32_t button)
		: m_button(button)
	{
	}

	int32_t IMouseButtonEvent::button() const
	{
		return m_button;
	}

	IEvent::ECategory IMouseButtonEvent::category() const
	{
		return IEvent::ECategory::Mouse | IEvent::ECategory::Input;
	}

	CMouseButtonPressedEvent::CMouseButtonPressedEvent(int32_t button)
		: IMouseButtonEvent(button)
	{
	}

	std::string CMouseButtonPressedEvent::name() const
	{
		return "MouseButtonPressed";
	}

	IEvent::EType CMouseButtonPressedEvent::type() const
	{
		return IEvent::EType::MouseButtonPressed;
	}

	CMouseButtonReleasedEvent::CMouseButtonReleasedEvent(int32_t button)
		: IMouseButtonEvent(button)
	{
	}

	std::string CMouseButtonReleasedEvent::name() const
	{
		return "MouseButtonReleased";
	}

	IEvent::EType CMouseButtonReleasedEvent::type() const
	{
		return IEvent::EType::MouseButtonReleased;
	}
} // namespace HyperCore
