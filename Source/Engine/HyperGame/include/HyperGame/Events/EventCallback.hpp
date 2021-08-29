/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <queue>

namespace HyperGame
{
	class IEventCallback
	{
	public:
		virtual ~IEventCallback() = default;

		virtual auto unregister_listener(uint32_t listener_id) -> bool = 0;
		virtual auto unregister_all_listeners() -> void = 0;

		virtual auto invoke_next() -> void = 0;
	};

	template <typename T>
	class EventCallback : public IEventCallback
	{
	public:
		auto register_listener(const std::function<void(const T&)>& listener) -> uint32_t
		{
			m_event_listeners[m_current_listener_id] = listener;
			return m_current_listener_id++;
		}

		auto unregister_listener(uint32_t listener_id) -> bool override
		{
			auto listener_iterator = m_event_listeners.find(listener_id);
			if (listener_iterator == m_event_listeners.end())
			{
				return false;
			}

			m_event_listeners.erase(listener_iterator);
			return true;
		}

		auto unregister_all_listeners() -> void override
		{
			m_event_listeners.clear();
		}

		template <typename... Args>
		auto push(Args&&... args) -> void
		{
			m_event_bus.emplace(T{ std::forward<Args>(args)... });
		}

		auto invoke_next() -> void override
		{
			const auto& event = m_event_bus.front();
			for (const auto& event_listener : m_event_listeners)
			{
				const auto& event_callback = event_listener.second;
				event_callback(event);
			}
			m_event_bus.pop();
		}

	private:
		std::unordered_map<uint32_t, std::function<void(const T&)>> m_event_listeners{};
		uint32_t m_current_listener_id{ 0 };

		std::queue<T> m_event_bus{};
	};
} // namespace HyperGame
