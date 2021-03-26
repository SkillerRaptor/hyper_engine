#pragma once

#include "Event.hpp"

namespace HyperEngine
{
	class WindowResizeEvent : public Event
	{
	private:
		uint32_t m_width;
		uint32_t m_height;
	
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_width{ width }
			, m_height{ height }
		{
		}
		
		inline uint32_t GetWidth() const { return m_width; }
		inline uint32_t GetHeight() const { return m_height; }
		
		virtual inline std::string GetName() const override { return "WindowResize"; }
		virtual inline Event::Type GetType() const override { return Event::Type::WindowResize; }
		virtual inline Event::Category GetCategory() const override { return Event::Category::WindowCategory; }
	};
	
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;
		
		virtual inline std::string GetName() const override { return "WindowClose"; }
		virtual inline Event::Type GetType() const override { return Event::Type::WindowClose; }
		virtual inline Event::Category GetCategory() const override { return Event::Category::WindowCategory; }
	};
	
	class WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() = default;
		
		virtual inline std::string GetName() const override { return "WindowFocus"; }
		virtual inline Event::Type GetType() const override { return Event::Type::WindowFocus; }
		virtual inline Event::Category GetCategory() const override { return Event::Category::WindowCategory; }
	};
	
	class WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() = default;
		
		virtual inline std::string GetName() const override { return "WindowLostFocus"; }
		virtual inline Event::Type GetType() const override { return Event::Type::WindowLostFocus; }
		virtual inline Event::Category GetCategory() const override { return Event::Category::WindowCategory; }
	};
	
	class WindowMovedEvent : public Event
	{
	private:
		uint32_t m_positionX;
		uint32_t m_positionY;
	
	public:
		WindowMovedEvent(uint32_t positionX, uint32_t positionY)
			: m_positionX{ positionX }
			, m_positionY{ positionY }
		{
		}
		
		inline uint32_t GetPositionX() const { return m_positionX; }
		inline uint32_t GetPositionY() const { return m_positionY; }
		
		virtual inline std::string GetName() const override { return "WindowMoved"; }
		virtual inline Event::Type GetType() const override { return Event::Type::WindowMoved; }
		virtual inline Event::Category GetCategory() const override { return Event::Category::WindowCategory; }
	};
}