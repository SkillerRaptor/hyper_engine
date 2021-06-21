/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Events/IEvent.hpp>

namespace HyperCore
{
	class CWindowResizeEvent : public IEvent
	{
	public:
		CWindowResizeEvent(uint32_t width, uint32_t height);
		virtual ~CWindowResizeEvent() override = default;

		uint32_t width() const;
		uint32_t height() const;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
		virtual IEvent::ECategory category() const override;

	private:
		uint32_t m_width;
		uint32_t m_height;
	};

	class CWindowCloseEvent : public IEvent
	{
	public:
		CWindowCloseEvent() = default;
		virtual ~CWindowCloseEvent() override = default;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
		virtual IEvent::ECategory category() const override;
	};

	class CWindowFocusEvent : public IEvent
	{
	public:
		CWindowFocusEvent() = default;
		virtual ~CWindowFocusEvent() override = default;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
		virtual IEvent::ECategory category() const override;
	};

	class CWindowLostFocusEvent : public IEvent
	{
	public:
		CWindowLostFocusEvent() = default;
		virtual ~CWindowLostFocusEvent() override = default;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
		virtual IEvent::ECategory category() const override;
	};

	class CWindowMovedEvent : public IEvent
	{
	public:
		CWindowMovedEvent(uint32_t position_x, uint32_t position_y);
		virtual ~CWindowMovedEvent() override = default;

		uint32_t position_x() const;
		uint32_t position_y() const;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
		virtual IEvent::ECategory category() const override;

	private:
		uint32_t m_position_x;
		uint32_t m_position_y;
	};
} // namespace HyperCore
