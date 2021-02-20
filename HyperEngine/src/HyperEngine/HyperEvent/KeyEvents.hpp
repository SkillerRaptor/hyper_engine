#pragma once

#include <sstream>

#include "HyperEvent/Event.hpp"

namespace HyperEvent
{
	class KeyEvent : public Event
	{
	protected:
		int m_KeyCode;

	public:
		inline int GetKeyCode() const
		{
			return m_KeyCode;
		}

		virtual inline int GetCategoryFlags() const override
		{
			return EventCategory::KeyCategory | EventCategory::InputCategory;
		}

	protected:
		KeyEvent(int keycode)
			: m_KeyCode{ keycode }
		{
		}
	};

	class KeyPressedEvent : public KeyEvent
	{
	private:
		int m_RepeatCount;

	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent{ keycode }, m_RepeatCount{ repeatCount }
		{
		}

		inline int GetRepeatCount() const
		{
			return m_RepeatCount;
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str().c_str();
		}

		virtual inline EventType GetEventType() const override
		{
			return EventType::KeyPressed;
		}

		virtual inline const char* GetName() const override
		{
			return "KeyPressed";
		}
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent{ keycode }
		{
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str().c_str();
		}

		virtual inline EventType GetEventType() const override
		{
			return EventType::KeyReleased;
		}

		virtual inline const char* GetName() const override
		{
			return "KeyReleased";
		}
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent{ keycode }
		{
		}

		inline const char* ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str().c_str();
		}

		virtual inline EventType GetEventType() const override
		{
			return EventType::KeyTyped;
		}

		virtual inline const char* GetName() const override
		{
			return "KeyTyped";
		}
	};
}
