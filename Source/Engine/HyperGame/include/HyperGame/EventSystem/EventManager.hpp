/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperGame/EventSystem/EventType.hpp"
#include "HyperGame/EventSystem/EventCallback.hpp"
#include "HyperGame/EventSystem/Listener.hpp"
#include "HyperGame/EventSystem/Events/KeyEvents.hpp"
#include "HyperGame/EventSystem/Events/MouseEvents.hpp"
#include "HyperGame/EventSystem/Events/WindowEvents.hpp"

#include <HyperCore/Logger.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <queue>
#include <type_traits>

namespace HyperEngine
{
	class EventManager
	{
	public:
		~EventManager();

		template <typename T>
		auto register_listener(const std::function<void(const T&)>& listener) -> Listener
		{
			constexpr auto event_id = EventType<T>::id();
			if (m_event_callbacks.find(event_id) == m_event_callbacks.end())
			{
				m_event_callbacks[event_id] = std::make_unique<EventCallback<T>>();
			}

			auto event_callback = static_cast<EventCallback<T>*>(m_event_callbacks[event_id].get());
			auto event_listener_id = event_callback->register_listener(listener);

			Listener listener_id = (static_cast<ListenerTraits::ListenerType>(event_id) << ListenerTraits::listener_shift) | event_listener_id;

			Logger::debug("Registered event listener with id #{}", listener_id);

			return listener_id;
		}

		auto unregister_listener(Listener id) -> bool;

		template <typename T, typename... Args>
		auto invoke(Args&&... args) -> void
		{
			constexpr auto event_id = EventType<T>::id();
			if (m_event_callbacks.find(event_id) == m_event_callbacks.end())
			{
				return;
			}

			auto event_callback = static_cast<EventCallback<T>*>(m_event_callbacks.at(event_id).get());
			event_callback->push(std::forward<Args>(args)...);

			m_event_queue.push(event_id);
		}

		auto process_events() -> void;

	private:
		std::unordered_map<uint32_t, std::unique_ptr<IEventCallback>> m_event_callbacks{};
		std::queue<uint32_t> m_event_queue{};
	};
} // namespace HyperEngine
