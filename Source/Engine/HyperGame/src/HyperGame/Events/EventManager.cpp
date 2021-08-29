/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperGame/Events/EventManager.hpp"

namespace HyperGame
{
	EventManager::~EventManager()
	{
		for (auto& event_pair : m_event_callbacks)
		{
			auto& event_callback = event_pair.second;
			event_callback->unregister_all_listeners();
		}
	}

	auto EventManager::process_events() -> void
	{
		if (m_event_queue.empty())
		{
			return;
		}

		const auto& event_id = m_event_queue.front();

		const auto& event_callback = m_event_callbacks[event_id];
		event_callback->invoke_next();

		m_event_queue.pop();
	}

	auto EventManager::unregister_listener(EventManager::ListenerId id) -> bool
	{
		auto event_id = static_cast<uint32_t>(id >> 32);
		if (m_event_callbacks.find(event_id) == m_event_callbacks.end())
		{
			return false;
		}

		auto event_callback = m_event_callbacks[event_id].get();
		event_callback->unregister_listener(static_cast<uint32_t>(id));

		HyperCore::Logger::debug("Unregistered event listener with id #{}", id);

		return true;
	}
} // namespace HyperGame
