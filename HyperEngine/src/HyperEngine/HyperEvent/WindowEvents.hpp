#pragma once

#include <sstream>

#include "HyperEvent/Event.hpp"

namespace HyperEvent
{
	class WindowResizeEvent : public Event
	{
	private:
		unsigned int m_Width;
		unsigned int m_Height;

	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width{ width }, m_Height{ height }
		{
		}

		inline unsigned int GetWidth() const
		{
			return m_Width;
		}

		inline unsigned int GetHeight() const
		{
			return m_Height;
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(WindowCategory)
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(WindowCategory)
	};

	class WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() = default;

		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEGORY(WindowCategory)
	};

	class WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() = default;

		EVENT_CLASS_TYPE(WindowLostFocus)
		EVENT_CLASS_CATEGORY(WindowCategory)
	};

	class WindowMovedEvent : public Event
	{
	private:
		unsigned int m_XPos;
		unsigned int m_YPos;

	public:
		WindowMovedEvent(unsigned int x, unsigned int y)
			: m_XPos{ x }, m_YPos{ y }
		{
		}

		inline int GetX() const
		{
			return m_XPos;
		}

		inline int GetY() const
		{
			return m_YPos;
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_XPos << ", " << m_YPos;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(WindowMoved)
		EVENT_CLASS_CATEGORY(WindowCategory)
	};
}
