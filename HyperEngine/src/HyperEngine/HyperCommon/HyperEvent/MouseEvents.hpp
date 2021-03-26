#pragma once

#include "Event.hpp"

namespace HyperEngine
{
	class MouseMovedEvent : public Event
	{
	private:
		float m_positionX;
		float m_positionY;
	
	public:
		MouseMovedEvent(float positionX, float positionY)
			: m_positionX{ positionX }
			, m_positionY{ positionY }
		{
		}
		
		inline float GetPositionX() const { return m_positionX; }
		inline float GetPositionY() const { return m_positionY; }
		
		virtual inline std::string GetName() const override { return "MouseMoved"; }
		virtual inline Event::Type GetType() const override { return Event::Type::MouseMoved; }
		virtual inline Event::Category GetCategory() const override
		{
			return static_cast<Event::Category>(Event::Category::MouseCategory | Event::Category::InputCategory);
		}
	};
	
	class MouseScrolledEvent : public Event
	{
	private:
		float m_offsetX;
		float m_offsetY;
	
	public:
		MouseScrolledEvent(float offsetX, float offsetY)
			: m_offsetX{ offsetX }
			, m_offsetY{ offsetY }
		{
		}
		
		inline float GetOffsetX() const { return m_offsetX; }
		inline float GetOffsetY() const { return m_offsetY; }
		
		virtual inline std::string GetName() const override { return "MouseScrolled"; }
		virtual inline Event::Type GetType() const override { return Event::Type::MouseScrolled; }
		virtual inline Event::Category GetCategory() const override
		{
			return static_cast<Event::Category>(Event::Category::MouseCategory | Event::Category::InputCategory);
		}
	};
	
	class MouseButtonEvent : public Event
	{
	protected:
		int m_button;
	
	public:
		inline int GetButton() const { return m_button; }
		
		virtual inline Event::Category GetCategory() const override
		{
			return static_cast<Event::Category>(Event::Category::MouseCategory | Event::Category::InputCategory);
		}
	
	protected:
		explicit MouseButtonEvent(int button)
			: m_button{ button }
		{
		}
	};
	
	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		explicit MouseButtonPressedEvent(int button)
			: MouseButtonEvent{ button }
		{
		}
		
		virtual inline std::string GetName() const override { return "MouseButtonPressed"; }
		virtual inline Event::Type GetType() const override { return Event::Type::MouseButtonPressed; }
	};
	
	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		explicit MouseButtonReleasedEvent(int button)
			: MouseButtonEvent{ button }
		{
		}
		
		virtual inline std::string GetName() const override { return "MouseButtonReleased"; }
		virtual inline Event::Type GetType() const override { return Event::Type::MouseButtonReleased; }
	};
}
