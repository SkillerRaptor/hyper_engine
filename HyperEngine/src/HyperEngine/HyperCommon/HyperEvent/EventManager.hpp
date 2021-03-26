#pragma once

#include "Event.hpp"
#include "EventWrapper.hpp"

#include <HyperCore/Log.hpp>

#include <unordered_map>
#include <vector>

namespace HyperEngine
{
	class EventTypeIdGenerator
	{
	public:
		template <typename>
		static size_t Type()
		{
			static const size_t id{ EventTypeIdGenerator::Identifier() };
			return id;
		}
	
	private:
		static size_t Identifier()
		{
			static size_t id{ 0 };
			return id++;
		}
	};
	
	class EventManager
	{
	private:
	
	public:
		EventManager() = default;
		~EventManager() = default;
		
		template <typename T>
		void InvokeEvent(const T& event)
		{
			static_assert(std::is_base_of_v<Event, T>, "Template argument is not a base class of Event");
			
			const size_t eventTypeId{ EventTypeIdGenerator::Type<T>() };
			if (m_eventWrappers.find(eventTypeId) == m_eventWrappers.end())
			{
				HYPERENGINE_CORE_WARNING("Unable to invoke the specified event type. The event type was never registered!");
				return;
			}
			
			std::shared_ptr<EventWrapperBase> pEventWrapperBase{ m_eventWrappers[eventTypeId] };
			std::shared_ptr<EventWrapper<T>> pEventWrapper{ std::static_pointer_cast<EventWrapper<T>>(pEventWrapperBase) };
			pEventWrapper->InvokeEvent(event);
		}
		
		template <typename T, typename... Args>
		void InvokeEvent(Args&&... args)
		{
			static_assert(std::is_base_of_v<Event, T>, "Template argument is not a base class of Event");
			
			const size_t eventTypeId{ EventTypeIdGenerator::Type<T>() };
			if (m_eventWrappers.find(eventTypeId) == m_eventWrappers.end())
			{
				return;
			}
			
			std::shared_ptr<EventWrapperBase> pEventWrapperBase{ m_eventWrappers[eventTypeId] };
			std::shared_ptr<EventWrapper<T>> pEventWrapper{ std::static_pointer_cast<EventWrapper<T>>(pEventWrapperBase) };
			pEventWrapper->InvokeEvent(T{ std::forward<Args>(args)... });
		}
		
		template <class T>
		void RegisterEventListener(const std::string& name, const std::function<void(const T&)>& eventListener)
		{
			static_assert(std::is_base_of_v<Event, T>, "Template argument is not a base class of Event");
			
			const size_t eventTypeId{ EventTypeIdGenerator::Type<T>() };
			if (m_eventWrappers.find(eventTypeId) == m_eventWrappers.end())
			{
				m_eventWrappers[eventTypeId] = std::make_shared<EventWrapper<T>>();
			}
			
			std::shared_ptr<EventWrapperBase> pEventWrapperBase{ m_eventWrappers[eventTypeId] };
			std::shared_ptr<EventWrapper<T>> pEventWrapper{ std::static_pointer_cast<EventWrapper<T>>(pEventWrapperBase) };
			pEventWrapper->RegisterEventListener(name, std::move(eventListener));
		}
		
		template <class T>
		void UnregisterEventListener(const std::string& name)
		{
			static_assert(std::is_base_of_v<Event, T>, "Template argument is not a base class of Event");
			
			const size_t eventTypeId{ EventTypeIdGenerator::Type<T>() };
			if (m_eventWrappers.find(eventTypeId) == m_eventWrappers.end())
			{
				HYPERENGINE_CORE_WARNING("Unable to unregister the specified event type. The event type was never registered!");
				return;
			}
			
			std::shared_ptr<EventWrapperBase> pEventWrapperBase{ m_eventWrappers[eventTypeId] };
			std::shared_ptr<EventWrapper<T>> pEventWrapper{ std::static_pointer_cast<EventWrapper<T>>(pEventWrapperBase) };
			pEventWrapper->UnregisterEventListener(name);
		}
	
	private:
		std::unordered_map<size_t, std::shared_ptr<EventWrapperBase>> m_eventWrappers;
	};
}