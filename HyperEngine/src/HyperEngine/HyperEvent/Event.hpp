#pragma once

#include <iostream>
#include <string>

#include "HyperEvent/EventManager.hpp"

namespace HyperEvent
{
	enum class EventType : uint8_t
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased,
		KeyPressed, KeyReleased, KeyTyped
	};

	enum EventCategory : uint8_t
	{
		None = 0,
		WindowCategory,
		MouseCategory,
		KeyCategory,
		InputCategory,
		CustomCategory
	};

	class Event
	{
	public:
		virtual ~Event() = default;

		virtual inline EventType GetEventType() const = 0;
		virtual inline const char* GetName() const = 0;
		virtual inline int GetCategoryFlags() const = 0;
		virtual inline const char* ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& event)
	{
		return os << event.ToString();
	}
}
