#pragma once

#include <iostream>

enum class EventType : uint8_t
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased,
	KeyPressed, KeyReleased, KeyTyped
};

enum EventCategory : uint8_t
{
	NoneCat = 0,
	WindowCategory,
	MouseCategory,
	KeyCategory,
	InputCategory,
	CustomCategory
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticEventType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticEventType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

class Event
{
public:
	virtual ~Event() = default;

	bool Handled = false;

	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

	bool IsInCategory(EventCategory category)
	{
		return GetCategoryFlags() & category;
	}
};

class EventDispatcher
{
private:
	Event& m_Event;

public:

	EventDispatcher(Event& event)
		: m_Event(event)
	{}

	template<typename T, typename F>
	bool Dispatch(const F& function)
	{
		if (m_Event.GetEventType() == T::GetStaticEventType())
		{
			m_Event.Handled = function(static_cast<T&>(m_Event));
			return true;
		}
		return false;
	}
};

inline std::ostream& operator<<(std::ostream& os, const Event& event)
{
	return os << event.ToString();
}