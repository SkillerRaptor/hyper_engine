/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Logger.hpp>
#include <HyperCore/Events/Event.hpp>
#include <HyperCore/Events/EventWrapper.hpp>
#include <HyperCore/Events/IEvent.hpp>
#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <queue>

namespace HyperCore
{
	class CEventFamilyGenerator
	{
	public:
		using EventIdType = uint16_t;

	public:
		template <typename>
		static EventIdType type()
		{
			static const EventIdType identifier = CEventFamilyGenerator::identifier();
			return identifier;
		}

	private:
		static EventIdType identifier();
	};

	class CEventManager
	{
	public:
		template <typename T>
		void invoke(const T& event)
		{
			static_assert(
				std::is_base_of_v<IEvent, T>,
				"Template argument T is not base of IEvent");
			
			m_event_bus.emplace(event);
		}

		template <typename T, typename... Args>
		void invoke(Args&&... args)
		{
			static_assert(
				std::is_base_of_v<IEvent, T>,
				"Template argument T is not base of IEvent");

			const CEventFamilyGenerator::EventIdType event_id = CEventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				m_event_wrappers[event_id] = std::make_unique<CEventWrapper<T>>();
			}

			m_event_bus.emplace(T(std::forward<Args>(args)...));
		}

		void process_next_event()
		{
			if (m_event_bus.empty())
			{
				return;
			}

			const CEvent& event = m_event_bus.front();

			switch (event.type())
			{
			case CEvent::EType::KeyPressed:
				invoke_next_event<SKeyPressedEvent>(event.as_key_pressed_event());
				break;
			case CEvent::EType::KeyReleased:
				invoke_next_event<SKeyReleasedEvent>(event.as_key_released_event());
				break;
			case CEvent::EType::KeyTyped:
				invoke_next_event<SKeyTypedEvent>(event.as_key_typed_event());
				break;
			case CEvent::EType::MouseMoved:
				invoke_next_event<SMouseMovedEvent>(event.as_mouse_moved_event());
				break;
			case CEvent::EType::MouseScrolled:
				invoke_next_event<SMouseScrolledEvent>(event.as_mouse_scrolled_event());
				break;
			case CEvent::EType::MouseButtonPressed:
				invoke_next_event<SMouseButtonPressedEvent>(event.as_mouse_button_pressed_event());
				break;
			case CEvent::EType::MouseButtonReleased:
				invoke_next_event<SMouseButtonReleasedEvent>(event.as_mouse_button_released_event());
				break;
			case CEvent::EType::WindowClose:
				invoke_next_event<SWindowCloseEvent>(event.as_window_close_event());
				break;
			case CEvent::EType::WindowResize:
				invoke_next_event<SWindowResizeEvent>(event.as_window_resize_event());
				break;
			case CEvent::EType::WindowFocus:
				invoke_next_event<SWindowFocusEvent>(event.as_window_focus_event());
				break;
			case CEvent::EType::WindowLostFocus:
				invoke_next_event<SWindowLostFocusEvent>(event.as_window_lost_focus_event());
				break;
			case CEvent::EType::WindowMoved:
				invoke_next_event<SWindowMovedEvent>(event.as_window_moved_event());
				break;
			default:
				break;
			}

			m_event_bus.pop();
		}

		template <class T>
		void register_listener(
			const std::string& name,
			const std::function<void(const T&)>& event_listener)
		{
			static_assert(
				std::is_base_of_v<IEvent, T>,
				"Template argument T is not base of IEvent");

			const CEventFamilyGenerator::EventIdType event_id = CEventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				m_event_wrappers[event_id] = std::make_unique<CEventWrapper<T>>();
			}

			CEventWrapper<T>* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->register_listener(name, event_listener);
		}

		template <class T>
		void unregister_listener(const std::string& name)
		{
			static_assert(
				std::is_base_of_v<IEvent, T>,
				"Template argument T is not base of IEvent");

			const CEventFamilyGenerator::EventIdType event_id = CEventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				CLogger::error("Failed to unregister event listener: '{}' not registered!", name);
				return;
			}

			CEventWrapper<T>* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->unregister_listener(name);
		}

	private:
		template <class T>
		CEventWrapper<T>*
			get_event_wrapper(CEventFamilyGenerator::EventIdType event_id)
		{
			const std::unique_ptr<IEventWrapperBase>& event_wrapper_base = m_event_wrappers[event_id];
			CEventWrapper<T>* event_wrapper = static_cast<CEventWrapper<T>*>(event_wrapper_base.get());
			return event_wrapper;
		}

		template <class T>
		void invoke_next_event(const T& event)
		{
			const CEventFamilyGenerator::EventIdType event_id = CEventFamilyGenerator::type<T>();

			CEventWrapper<T>* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->invoke(event);
		}

	private:
		std::unordered_map<
			CEventFamilyGenerator::EventIdType,
			std::unique_ptr<IEventWrapperBase>>
			m_event_wrappers{};

		std::queue<CEvent> m_event_bus{};
	};
} // namespace HyperCore
