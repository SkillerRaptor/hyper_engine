#pragma once

#include "Event.hpp"

#include <HyperCore/Log.hpp>

#include <functional>
#include <string>
#include <vector>

namespace HyperEngine
{
	class EventWrapperBase
	{
	public:
		EventWrapperBase() = default;
		virtual ~EventWrapperBase() = default;
	};
	
	template <typename T>
	class EventWrapper : public EventWrapperBase
	{
	public:
		static_assert(std::is_base_of_v<Event, T>, "Template argument is not a base class of Event");
	
	private:
		struct EventListener
		{
			std::string name;
			std::function<void(const T&)> eventListenerFunction;
		};
		
	public:
		EventWrapper() = default;
		virtual ~EventWrapper() = default;
		
		inline void InvokeEvent(const T& event) const
		{
			for (const EventListener& eventListener : m_eventListeners)
			{
				eventListener.eventListenerFunction(event);
			}
		}
		
		inline void RegisterEventListener(const std::string& name, const std::function<void(const T&)>& eventListenerFunction)
		{
			for (const EventListener& eventListener : m_eventListeners)
			{
				if (eventListener.name == name)
				{
					HYPERENGINE_CORE_ERROR("Failed to register event listener! The name was already used!");
					return;
				}
			}
			
			EventListener eventListener{};
			eventListener.name = name;
			eventListener.eventListenerFunction = std::move(eventListenerFunction);
			m_eventListeners.push_back(std::move(eventListener));
		}
		
		inline void UnregisterEventListener(const std::string& name, const std::function<void(const T&)>& eventCallback)
		{
			for (typename std::vector<EventListener>::iterator it = m_eventListeners.begin(); it != m_eventListeners.end(); it++)
			{
				EventListener& eventListener{ *it };
				if (eventListener.name == name)
				{
					m_eventListeners.erase(it);
					return;
				}
			}
			
			HYPERENGINE_CORE_ERROR("Failed to unregister event listener! The specified name was not found!");
		}
	
	private:
		std::vector<EventListener> m_eventListeners;
	};
}