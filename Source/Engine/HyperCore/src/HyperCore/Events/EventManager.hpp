/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Logger.hpp>
#include <HyperCore/Events/IEvent.hpp>
#include <HyperCore/Events/EventWrapper.hpp>
#include <HyperCore/Events/KeyEvents.hpp>
#include <HyperCore/Events/MouseEvents.hpp>
#include <HyperCore/Events/WindowEvents.hpp>
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
			static const EventIdType identifier =
				CEventFamilyGenerator::identifier();
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

			const CEventFamilyGenerator::EventIdType event_id =
				CEventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				m_event_wrappers[event_id] =
					std::make_unique<CEventWrapper<T>>();
			}

			m_event_bus.push(std::make_unique<T>(event));
		}

		template <typename T, typename... Args>
		void invoke(Args&&... args)
		{
			static_assert(
				std::is_base_of_v<IEvent, T>,
				"Template argument T is not base of IEvent");

			const CEventFamilyGenerator::EventIdType event_id =
				CEventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				m_event_wrappers[event_id] =
					std::make_unique<CEventWrapper<T>>();
			}

			m_event_bus.push(std::make_unique<T>(std::forward<Args>(args)...));
		}

		void process_next_event()
		{
			if (m_event_bus.empty())
			{
				return;
			}

			std::unique_ptr<IEvent>& event = m_event_bus.front();

			switch (event->type())
			{
			case IEvent::EType::KeyPressed:
				invoke_next_event<CKeyPressedEvent>(event);
				break;
			case IEvent::EType::KeyReleased:
				invoke_next_event<CKeyReleasedEvent>(event);
				break;
			case IEvent::EType::KeyTyped:
				invoke_next_event<CKeyTypedEvent>(event);
				break;
			case IEvent::EType::MouseMoved:
				invoke_next_event<CMouseMovedEvent>(event);
				break;
			case IEvent::EType::MouseScrolled:
				invoke_next_event<CMouseScrolledEvent>(event);
				break;
			case IEvent::EType::MouseButtonPressed:
				invoke_next_event<CMouseButtonPressedEvent>(event);
				break;
			case IEvent::EType::MouseButtonReleased:
				invoke_next_event<CMouseButtonReleasedEvent>(event);
				break;
			case IEvent::EType::WindowClose:
				invoke_next_event<CWindowCloseEvent>(event);
				break;
			case IEvent::EType::WindowResize:
				invoke_next_event<CWindowResizeEvent>(event);
				break;
			case IEvent::EType::WindowFocus:
				invoke_next_event<CWindowFocusEvent>(event);
				break;
			case IEvent::EType::WindowLostFocus:
				invoke_next_event<CWindowLostFocusEvent>(event);
				break;
			case IEvent::EType::WindowMoved:
				invoke_next_event<CWindowMovedEvent>(event);
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

			const CEventFamilyGenerator::EventIdType event_id =
				CEventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				m_event_wrappers[event_id] =
					std::make_unique<CEventWrapper<T>>();
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

			const CEventFamilyGenerator::EventIdType event_id =
				CEventFamilyGenerator::type<T>();
			if (m_event_wrappers.find(event_id) == m_event_wrappers.end())
			{
				CLogger::error(
					"Failed to unregister event listener: '{}' not registered!",
					name);
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
			const std::unique_ptr<IEventWrapperBase>& event_wrapper_base =
				m_event_wrappers[event_id];
			CEventWrapper<T>* event_wrapper =
				static_cast<CEventWrapper<T>*>(event_wrapper_base.get());
			return event_wrapper;
		}

		template <class T>
		void invoke_next_event(const std::unique_ptr<IEvent>& event)
		{
			const CEventFamilyGenerator::EventIdType event_id =
				CEventFamilyGenerator::type<T>();

			CEventWrapper<T>* event_wrapper = get_event_wrapper<T>(event_id);
			event_wrapper->invoke(*static_cast<T*>(event.get()));
		}

	private:
		std::unordered_map<
			CEventFamilyGenerator::EventIdType,
			std::unique_ptr<IEventWrapperBase>>
			m_event_wrappers{};

		std::queue<std::unique_ptr<IEvent>> m_event_bus{};
	};
} // namespace HyperCore
