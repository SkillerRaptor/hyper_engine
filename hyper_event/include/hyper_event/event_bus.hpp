/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "hyper_event/event_handler.hpp"
#include "hyper_event/event_id_generator.hpp"

namespace hyper_event
{
    class EventBus
    {
    public:
        template <typename T>
        void subscribe(const std::function<void(const T &)> &callback)
        {
            const size_t event_id = EventIdGenerator::type<T>();
            if (!m_handlers.contains(event_id))
            {
                m_handlers.insert({ event_id, std::make_unique<EventHandlerImpl<T>>() });
            }

            const std::unique_ptr<EventHandler> &handler = m_handlers[event_id];
            auto *event_handler = static_cast<EventHandlerImpl<T> *>(handler.get());
            event_handler->subscribe(callback);
        }

        template <typename T>
        void dispatch(const T &event)
        {
            const size_t event_id = EventIdGenerator::type<T>();
            if (!m_handlers.contains(event_id))
            {
                return;
            }

            const std::unique_ptr<EventHandler> &handler = m_handlers[event_id];
            auto *event_handler = static_cast<EventHandlerImpl<T> *>(handler.get());
            event_handler->dispatch(event);
        }

    private:
        std::unordered_map<size_t, std::unique_ptr<EventHandler>> m_handlers;
    };
} // namespace hyper_event
