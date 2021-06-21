/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Events/IEvent.hpp>

namespace HyperCore
{
	class CMouseMovedEvent : public IEvent
	{
	public:
		CMouseMovedEvent(float position_x, float position_y);
		virtual ~CMouseMovedEvent() override = default;

		float position_x() const;
		float position_y() const;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
		virtual IEvent::ECategory category() const override;

	private:
		float m_position_x;
		float m_position_y;
	};

	class CMouseScrolledEvent : public IEvent
	{
	public:
		CMouseScrolledEvent(float offset_x, float offset_y);
		virtual ~CMouseScrolledEvent() override = default;

		float offset_x() const;
		float offset_y() const;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
		virtual IEvent::ECategory category() const override;

	private:
		float m_offset_x;
		float m_offset_y;
	};

	class IMouseButtonEvent : public IEvent
	{
	public:
		virtual ~IMouseButtonEvent() override = default;

		int32_t button() const;

		virtual IEvent::ECategory category() const override;

	protected:
		explicit IMouseButtonEvent(int32_t button);

	protected:
		int32_t m_button;
	};

	class CMouseButtonPressedEvent : public IMouseButtonEvent
	{
	public:
		explicit CMouseButtonPressedEvent(int32_t button);
		virtual ~CMouseButtonPressedEvent() override = default;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
	};

	class CMouseButtonReleasedEvent : public IMouseButtonEvent
	{
	public:
		explicit CMouseButtonReleasedEvent(int32_t button);
		virtual ~CMouseButtonReleasedEvent() override = default;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
	};
} // namespace HyperCore
