/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Logger.hpp>
#include <functional>
#include <string>
#include <vector>

namespace HyperCore
{
	class IEventWrapperBase
	{
	public:
		virtual ~IEventWrapperBase() = default;
	};

	template <typename T>
	struct SEventListener
	{
		std::string name;
		std::function<void(const T&)> function;
	};

	template <class T>
	class CEventWrapper : public IEventWrapperBase
	{
	public:
		static_assert(
			std::is_base_of_v<IEvent, T>,
			"Template argument T is not base of IEvent");

		void invoke(const T& event) const
		{
			for (const SEventListener<T>& event_listener : m_event_listeners)
			{
				event_listener.function(event);
			}
		}

		void register_listener(
			const std::string& name,
			const std::function<void(const T&)>& eventListenerFunction)
		{
			for (const SEventListener<T>& event_listener : m_event_listeners)
			{
				if (m_event_listeners.name == name)
				{
					CLogger::error("Failed to register event listener: name "
								   "already in use!");
					return;
				}
			}

			SEventListener<T> event_listener{};
			event_listener.name = name;
			event_listener.function = std::move(eventListenerFunction);
			m_event_listeners.push_back(std::move(event_listener));
		}

		void unregister_listener(const std::string& name)
		{
			for (typename std::vector<SEventListener<T>>::iterator it =
					 m_event_listeners.begin();
				 it != m_event_listeners.end();
				 ++it)
			{
				if (it->name == name)
				{
					m_event_listeners.erase(it);
					return;
				}
			}

			CLogger::error(
				"Failed to unregister event listener: '{}' not registered!",
				name);
		}

	private:
		std::vector<SEventListener<T>> m_event_listeners{};
	};
} // namespace HyperCore
