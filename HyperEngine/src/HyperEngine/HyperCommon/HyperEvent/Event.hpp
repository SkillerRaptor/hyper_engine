#pragma once

#include <string>

namespace HyperEngine
{
	class Event
	{
	public:
		using CategoryIndex = uint8_t;
		using TypeIndex = uint8_t;
		
		enum Category : CategoryIndex
		{
			None = 0,
			WindowCategory,
			MouseCategory,
			KeyCategory,
			InputCategory
		};
		
		enum class Type : TypeIndex
		{
			None = 0,
			WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
			MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased,
			KeyPressed, KeyReleased, KeyTyped
		};
		
	public:
		Event() = default;
		virtual ~Event() = default;

		virtual inline std::string GetName() const = 0;
		virtual inline Type GetType() const = 0;
		virtual inline Category GetCategory() const = 0;
	};
}
