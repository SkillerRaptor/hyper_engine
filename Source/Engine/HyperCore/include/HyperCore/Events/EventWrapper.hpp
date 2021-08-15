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
		auto invoke(const T& event) const -> void
		{
			for (const auto& event_listener : m_event_listeners)
			{
				event_listener.callback(event);
			}
		}

		auto register_listener(const std::string& name, const std::function<void(const T&)>& callback) -> void
		{
			for (const auto& event_listener : m_event_listeners)
			{
				if (event_listener.name == name)
				{
					return;
				}
			}

			EventListener<T> event_listener{};
			event_listener.name = name;
			event_listener.callback = std::move(callback);
			m_event_listeners.emplace_back(std::move(event_listener));
		}

		auto unregister_listener(const std::string& name) -> void
		{
			auto it = m_event_listeners.begin();
			while (it != m_event_listeners.end())
			{
				if (it->name == name)
				{
					it = m_event_listeners.erase(it);
					continue;
				}
				
				++it;
			}
		}

	private:
		std::vector<EventListener<T>> m_event_listeners{};
	};
} // namespace HyperCore
