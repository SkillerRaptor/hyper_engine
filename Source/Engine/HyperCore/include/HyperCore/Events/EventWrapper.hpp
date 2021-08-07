/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Logger.hpp"

#include <functional>
#include <string>
#include <vector>

namespace HyperCore
{
	class EventWrapperBase
	{
	public:
		virtual ~EventWrapperBase() = default;
	};

	template <typename T>
	struct EventListener
	{
		std::string name;
		std::function<void(const T&)> callback;
	};

	template <class T>
	class EventWrapper : public EventWrapperBase
	{
	public:
		void invoke(const T& event) const
		{
			for (const EventListener<T>& event_listener : m_event_listeners)
			{
				event_listener.callback(event);
			}
		}

		void register_listener(const std::string& name, const std::function<void(const T&)>& callback)
		{
			for (const EventListener<T>& event_listener : m_event_listeners)
			{
				if (event_listener.name == name)
				{
					Logger::error("Failed to register event listener: name already in use!");
					return;
				}
			}

			EventListener<T> event_listener{};
			event_listener.name = name;
			event_listener.callback = std::move(callback);
			m_event_listeners.push_back(std::move(event_listener));
		}

		void unregister_listener(const std::string& name)
		{
			for (typename std::vector<EventListener<T>>::iterator it = m_event_listeners.begin();
				 it != m_event_listeners.end();
				 ++it)
			{
				if (it->name == name)
				{
					m_event_listeners.erase(it);
					return;
				}
			}

			Logger::error("Failed to unregister event listener: '{}' not registered!", name);
		}

	private:
		std::vector<EventListener<T>> m_event_listeners{};
	};
} // namespace HyperCore
