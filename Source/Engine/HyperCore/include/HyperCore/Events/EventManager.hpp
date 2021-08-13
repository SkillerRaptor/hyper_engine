/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Logger.hpp"
#include "HyperCore/Events/Event.hpp"
#include "HyperCore/Events/EventWrapper.hpp"

#include <memory>
#include <unordered_map>
#include <queue>

namespace HyperCore
{
	// TODO: Refactoring Events
	class EventFamilyGenerator
	{
	public:
		using EventIdType = uint16_t;

	public:
		template <typename>
		static EventIdType type()
		{
			static const EventIdType identifier = EventFamilyGenerator::identifier();
			return identifier;
		}

	private:
		static EventIdType identifier();
	};

	class EventManager
	{
	public:
		template <typename T>
		void invoke(const T& event)
		{
			m_event_bus.emplace(event);
		}

		template <typename T, typename... Args>
		void invoke(Args&&... args)
		{
			const EventFamilyGenerator::EventIdType event_id = EventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				m_event_wrappers[event_id] = std::make_unique<EventWrapper<T>>();
			}

			m_event_bus.emplace(T{ std::forward<Args>(args)... });
		}

		void process_next_event()
		{
			if (m_event_bus.empty())
			{
				return;
			}

			const Event& event = m_event_bus.front();

			switch (event.type())
			{
			case Event::Type::KeyPressed:
				invoke_next_event<KeyPressedEvent>(event.as_key_pressed_event());
				break;
			case Event::Type::KeyReleased:
				invoke_next_event<KeyReleasedEvent>(event.as_key_released_event());
				break;
			case Event::Type::KeyTyped:
				invoke_next_event<KeyTypedEvent>(event.as_key_typed_event());
				break;
			case Event::Type::MouseMoved:
				invoke_next_event<MouseMovedEvent>(event.as_mouse_moved_event());
				break;
			case Event::Type::MouseScrolled:
				invoke_next_event<MouseScrolledEvent>(event.as_mouse_scrolled_event());
				break;
			case Event::Type::MouseButtonPressed:
				invoke_next_event<MouseButtonPressedEvent>(event.as_mouse_button_pressed_event());
				break;
			case Event::Type::MouseButtonReleased:
				invoke_next_event<MouseButtonReleasedEvent>(event.as_mouse_button_released_event());
				break;
			case Event::Type::WindowClose:
				invoke_next_event<WindowCloseEvent>(event.as_window_close_event());
				break;
			case Event::Type::WindowResize:
				invoke_next_event<WindowResizeEvent>(event.as_window_resize_event());
				break;
			case Event::Type::WindowFocus:
				invoke_next_event<WindowFocusEvent>(event.as_window_focus_event());
				break;
			case Event::Type::WindowLostFocus:
				invoke_next_event<WindowLostFocusEvent>(event.as_window_lost_focus_event());
				break;
			case Event::Type::WindowMoved:
				invoke_next_event<WindowMovedEvent>(event.as_window_moved_event());
				break;
			default:
				break;
			}

			m_event_bus.pop();
		}

		template <class T>
		void register_listener(const std::string& name, const std::function<void(const T&)>& event_listener)
		{
			const EventFamilyGenerator::EventIdType event_id = EventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				m_event_wrappers[event_id] = std::make_unique<EventWrapper<T>>();
			}

			EventWrapper<T>* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->register_listener(name, event_listener);
		}

		template <class T>
		void unregister_listener(const std::string& name)
		{
			const EventFamilyGenerator::EventIdType event_id = EventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				Logger::error("Failed to unregister event listener: '{}' not registered!", name);
				return;
			}

			EventWrapper<T>* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->unregister_listener(name);
		}

	private:
		template <class T>
		EventWrapper<T>* get_event_wrapper(EventFamilyGenerator::EventIdType event_id)
		{
			const std::unique_ptr<EventWrapperBase>& event_wrapper_base = m_event_wrappers[event_id];
			auto* event_wrapper = static_cast<EventWrapper<T>*>(event_wrapper_base.get());
			return event_wrapper;
		}

		template <class T>
		void invoke_next_event(const T& event)
		{
			const EventFamilyGenerator::EventIdType event_id = EventFamilyGenerator::type<T>();

			EventWrapper<T>* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->invoke(event);
		}

	private:
		std::unordered_map<EventFamilyGenerator::EventIdType, std::unique_ptr<EventWrapperBase>> m_event_wrappers{};

		std::queue<Event> m_event_bus{};
	};
} // namespace HyperCore
