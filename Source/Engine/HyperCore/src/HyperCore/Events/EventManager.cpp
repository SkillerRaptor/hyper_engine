/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Events/EventManager.hpp"

namespace HyperCore
{
	EventManager::EventManager()
	{
		register_event<KeyPressedEvent>();
		register_event<KeyReleasedEvent>();
		register_event<MouseMovedEvent>();
		register_event<MouseScrolledEvent>();
		register_event<MouseButtonPressedEvent>();
		register_event<MouseButtonReleasedEvent>();
		register_event<WindowCloseEvent>();
		register_event<WindowResizeEvent>();
		register_event<WindowFramebufferResizeEvent>();
		register_event<WindowFocusEvent>();
		register_event<WindowLostFocusEvent>();
		register_event<WindowMovedEvent>();
	}

	EventManager::~EventManager()
	{
		for (auto& event_wrapper : m_event_wrappers)
		{
			delete event_wrapper.second;
		}
	}

	void EventManager::process_next_event()
	{
		if (m_event_bus.empty())
		{
			return;
		}

		const auto& event = m_event_bus.front();
		std::visit([this](auto&& converted_event)
		   {
			   using T = std::decay_t<decltype(converted_event)>;
			   invoke_event<T>(converted_event);
		   }, event);

		m_event_bus.pop();
	}
} // namespace HyperCore
