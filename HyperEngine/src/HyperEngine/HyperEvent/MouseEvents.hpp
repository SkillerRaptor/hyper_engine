#pragma once

#include <sstream>

#include "HyperEvent/Event.hpp"

namespace HyperEvent
{
	class MouseMovedEvent : public Event
	{
	private:
		float m_MouseX, m_MouseY;

	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX{ x }, m_MouseY{ y }
		{
		}

		inline float GetMouseX() const
		{
			return m_MouseX;
		}

		inline float GetMouseY() const
		{
			return m_MouseY;
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(MouseCategory | InputCategory)
	};

	class MouseScrolledEvent : public Event
	{
	private:
		float m_XOffset, m_YOffset;

	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset{ xOffset }, m_YOffset{ yOffset }
		{
		}

		inline float GetXOffset() const
		{
			return m_XOffset;
		}

		inline float GetYOffset() const
		{
			return m_YOffset;
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(MouseCategory | InputCategory)
	};

	class MouseButtonEvent : public Event
	{
	protected:
		int m_Button;

	public:
		inline int GetMouseButton() const
		{
			return m_Button;
		}

		EVENT_CLASS_CATEGORY(MouseCategory | InputCategory)

	protected:
		MouseButtonEvent(int button)
			: m_Button{ button }
		{
		}
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent{ button }
		{
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent{ button }
		{
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}
