/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Compilers.hpp"
#include "HyperCore/Hasher.hpp"
#include "HyperCore/Events/Event.hpp"
#include "HyperCore/Events/EventWrapper.hpp"

#include <memory>
#include <unordered_map>
#include <queue>

namespace HyperCore
{
	using EventIdType = unsigned int;

	template <typename T>
	class EventType
	{
	public:
		static constexpr auto id() -> EventIdType
		{
			constexpr EventIdType value = Hasher::hash_crc32(HYPERENGINE_FUNCTION_SIGNATURE);
			return value;
		}
	};

	class EventManager
	{
	public:
		EventManager();
		~EventManager();

		template <typename T>
		auto invoke(const T& event) -> void
		{
			m_event_bus.emplace(event);
		}

		template <typename T, typename... Args>
		auto invoke(Args&&... args) -> void
		{
			m_event_bus.emplace(T{ std::forward<Args>(args)... });
		}

		template <typename T>
		auto register_listener(const std::string& name, const std::function<void(const T&)>& event_listener) -> void
		{
			constexpr EventIdType event_id = EventType<T>::id();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				return;
			}

			auto* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->register_listener(name, event_listener);
		}

		template <class T>
		auto unregister_listener(const std::string& name) -> void
		{
			constexpr EventIdType event_id = EventType<T>::id();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				return;
			}

			auto* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->unregister_listener(name);
		}

		auto process_next_event() -> void;

	private:
		template <typename T>
		auto get_event_wrapper(EventIdType event_id) -> EventWrapper<T>*
		{
			auto* event_wrapper_base = m_event_wrappers[event_id];
			auto* event_wrapper = static_cast<EventWrapper<T>*>(event_wrapper_base);
			return event_wrapper;
		}

		template <typename T>
		auto register_event() -> void
		{
			constexpr EventIdType event_id = EventType<T>::id();
			m_event_wrappers[event_id] = new EventWrapper<T>();
		}

		template <typename T>
		auto invoke_event(const T& event) -> void
		{
			constexpr EventIdType event_id = EventType<T>::id();
			auto* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->invoke(event);
		}

	private:
		std::unordered_map<EventIdType, EventWrapperBase*> m_event_wrappers{};
		std::queue<Event> m_event_bus{};
	};
} // namespace HyperCore
