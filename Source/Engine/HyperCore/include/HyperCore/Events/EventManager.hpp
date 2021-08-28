/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Logger.hpp"
#include "HyperCore/Events/EventType.hpp"
#include "HyperCore/Events/EventCallback.hpp"
#include "HyperCore/Events/KeyEvents.hpp"
#include "HyperCore/Events/MouseEvents.hpp"
#include "HyperCore/Events/WindowEvents.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <queue>

namespace HyperCore
{
	class EventManager
	{
	public:
		using ListenerId = uint64_t;
		
	public:
		~EventManager();
		
		template <typename T>
		auto register_listener(const std::function<void(const T&)>& listener) -> ListenerId
		{
			constexpr auto event_id = EventType<T>::id();
			if (m_event_callbacks.find(event_id) == m_event_callbacks.end())
			{
				m_event_callbacks[event_id] = std::make_unique<EventCallback<T>>();
			}
			
			auto event_callback = static_cast<EventCallback<T>*>(m_event_callbacks[event_id].get());
			auto event_listener_id = event_callback->register_listener(listener);
			
			ListenerId listener_id = (static_cast<ListenerId>(event_id) << 32) | event_listener_id;
			
			HyperCore::Logger::debug("Registered event listener with id #{}", listener_id);
			
			return listener_id;
		}
		
		auto unregister_listener(ListenerId id) -> bool;

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
} // namespace HyperCore
