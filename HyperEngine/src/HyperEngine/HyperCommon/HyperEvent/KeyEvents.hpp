#pragma once

#include "Event.hpp"

namespace HyperEngine
{
	class KeyEvent : public Event
	{
	protected:
		int m_keyCode;
	
	public:
		inline int GetKeyCode() const { return m_keyCode; }
		
		virtual inline Event::Category GetCategory() const override
		{
			return static_cast<Event::Category>(Event::Category::KeyCategory | Event::Category::InputCategory);
		}
	
	protected:
		explicit KeyEvent(int keycode)
			: m_keyCode{ keycode }
		{
		}
	};
	
	class KeyPressedEvent : public KeyEvent
	{
	private:
		int m_repeatCount;
	
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent{ keycode }
			, m_repeatCount{ repeatCount }
		{
		}
		
		inline int GetRepeatCount() const { return m_repeatCount; }
		
		virtual inline std::string GetName() const override { return "KeyPressed"; }
		virtual inline Event::Type GetType() const override { return Event::Type::KeyPressed; }
	};
	
	class KeyReleasedEvent : public KeyEvent
	{
	public:
		explicit KeyReleasedEvent(int keycode)
			: KeyEvent{ keycode }
		{
		}
		
		virtual inline std::string GetName() const override { return "KeyReleased"; }
		virtual inline Event::Type GetType() const override { return Event::Type::KeyReleased; }
	};
	
	class KeyTypedEvent : public KeyEvent
	{
	public:
		explicit KeyTypedEvent(int keycode)
			: KeyEvent{ keycode }
		{
		}
		
		virtual inline std::string GetName() const override { return "KeyTyped"; }
		virtual inline Event::Type GetType() const override { return Event::Type::KeyTyped; }
	};
}
