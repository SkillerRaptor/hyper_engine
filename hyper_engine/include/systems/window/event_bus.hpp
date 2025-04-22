/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "systems/window/event_handler.hpp"

class EventBus
{
private:
    class EventIdGenerator
    {
    public:
        template <typename>
        static size_t type()
        {
            static const size_t value = identifier();
            return value;
        }

    private:
        static size_t identifier()
        {
            static size_t value = 0;
            return value++;
        }
    };

public:
    template <typename T, typename... Args>
    void dispatch(Args &&...args)
    {
        const size_t event_id = EventIdGenerator::type<T>();
        if (!m_handlers.contains(event_id))
        {
            return;
        }

        const std::unique_ptr<EventHandler> &handler = m_handlers.at(event_id);
        EventHandlerImpl<T> *event_handler = static_cast<EventHandlerImpl<T> *>(handler.get());
        event_handler->dispatch(T(std::forward<Args>(args)...));
    }

    // FIXME: To prevent functions being called which are no longer available, add an id system to unsubscribe event handlers
    template <typename T>
    void subscribe(const std::function<void(const T &)> &callback)
    {
        const size_t event_id = EventIdGenerator::type<T>();
        if (!m_handlers.contains(event_id))
        {
            m_handlers.insert({event_id, std::make_unique<EventHandlerImpl<T>>()});
        }

        const std::unique_ptr<EventHandler> &handler = m_handlers.at(event_id);
        EventHandlerImpl<T> *event_handler = static_cast<EventHandlerImpl<T> *>(handler.get());
        event_handler->subscribe(callback);
    }

private:
    std::unordered_map<size_t, std::unique_ptr<EventHandler>> m_handlers;
};
