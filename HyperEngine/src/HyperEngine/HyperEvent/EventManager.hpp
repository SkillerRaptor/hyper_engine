#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

namespace HyperEvent
{
	class Event;

	class EventIdGenerator
	{
	public:
		template<typename>
		static std::size_t Type() noexcept
		{
			static const std::size_t value = Identifier();
			return value;
		}

	private:
		static std::size_t Identifier() noexcept
		{
			static std::size_t value = 0;
			return value++;
		}
	};

	class EventWrapper
	{
	public:
		EventWrapper() = default;
		virtual ~EventWrapper() = default;
	};

	template <typename T>
	class EventWrapperImpl : public EventWrapper
	{
	public:
		static_assert(std::is_base_of<Event, T>::value, "Template Argument T is not Base of Event");

	private:
		std::vector<std::function<void(const T&)>> m_EventCallbacks;

	public:
		EventWrapperImpl() = default;
		~EventWrapperImpl() = default;

		inline void RegisterEventCallback(const std::function<void(const T&)> eventCallback)
		{
			m_EventCallbacks.emplace_back(std::move(eventCallback));
		}

		inline void CallEventCallbacks(const T& event) const
		{
			for (const std::function<void(const T&)>& function : m_EventCallbacks)
				function(event);
		}
	};

	class EventManager
	{
	private:
		std::unordered_map<size_t, std::shared_ptr<EventWrapper>> m_EventWrappers;

	public:
		template <typename T>
		void RegisterEventCallback(const std::function<void(const T&)>& callback)
		{
			static_assert(std::is_base_of<Event, T>::value, "Template Argument T is not Base of Event");

			size_t typeId = EventIdGenerator::Type<T>();
			if (m_EventWrappers.find(typeId) == m_EventWrappers.end())
				m_EventWrappers[typeId] = std::move(std::make_shared<EventWrapperImpl<T>>());

			std::shared_ptr<EventWrapperImpl<T>> eventWrapper = std::static_pointer_cast<EventWrapperImpl<T>>(m_EventWrappers[typeId]);
			eventWrapper->RegisterEventCallback(std::move(callback));
		}

		template<typename T>
		void CallEvent(const T& event)
		{
			static_assert(std::is_base_of<Event, T>::value, "Template Argument T is not Base of Event");

			size_t typeId = EventIdGenerator::Type<T>();
			if (m_EventWrappers.find(typeId) == m_EventWrappers.end())
				return;

			std::shared_ptr<EventWrapperImpl<T>> eventWrapper = std::static_pointer_cast<EventWrapperImpl<T>>(m_EventWrappers[typeId]);
			eventWrapper->CallEventCallbacks(event);
		}

		template<typename T, typename... Args>
		void CallEvent(Args&&... args)
		{
			static_assert(std::is_base_of<Event, T>::value, "Template Argument T is not Base of Event");

			size_t typeId = EventIdGenerator::Type<T>();
			if (m_EventWrappers.find(typeId) == m_EventWrappers.end())
				return;

			std::shared_ptr<EventWrapperImpl<T>> eventWrapper = std::static_pointer_cast<EventWrapperImpl<T>>(m_EventWrappers[typeId]);
			eventWrapper->CallEventCallbacks(T{ std::forward<Args>(args)... });
		}
	};
}
