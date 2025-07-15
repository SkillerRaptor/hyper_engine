/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>
#include <memory>

#include "core/prerequisites.hpp"
#include "core/types.hpp"

class EventServer
{
private:
    HE_NON_COPYABLE(EventServer);
    HE_NON_MOVABLE(EventServer);

private:
    class IdGenerator
    {
    public:
        template <typename>
        static usize type()
        {
            static const usize value = identifier();
            return value;
        }

    private:
        static usize identifier()
        {
            static usize value { 0 };
            return value++;
        }
    };

    class Handler
    {
    public:
        virtual ~Handler() = default;
    };

    template <typename T>
    class HandlerImpl final : public Handler
    {
    private:
        using Callback = std::function<void(const T &)>;

    public:
        void subscribe(const Callback &callback) { m_callbacks.push_back(callback); }

        void dispatch(const T &event)
        {
            for (const Callback &callback : m_callbacks)
            {
                callback(event);
            }
        }

    private:
        std::vector<Callback> m_callbacks {};
    };

public:
    static std::unique_ptr<EventServer> create();
    ~EventServer();

    void poll();

    // FIXME: Add id system to unsubscribe event handlers
    template <typename T>
    void subscribe(const std::function<void(const T &)> &callback)
    {
        const usize id = IdGenerator::type<T>();
        if (!m_handlers.contains(id))
        {
            m_handlers.insert({ id, std::make_unique<HandlerImpl<T>>() });
        }

        const std::unique_ptr<Handler> &handler = m_handlers.at(id);
        HandlerImpl<T> *event_handler = static_cast<HandlerImpl<T> *>(handler.get());
        event_handler->subscribe(callback);
    }

private:
    template <typename T, typename... Args>
    void dispatch(Args &&...args)
    {
        const usize id = IdGenerator::type<T>();
        if (!m_handlers.contains(id))
        {
            return;
        }

        const std::unique_ptr<Handler> &handler = m_handlers.at(id);
        HandlerImpl<T> *event_handler = static_cast<HandlerImpl<T> *>(handler.get());
        event_handler->dispatch(T { std::forward<Args>(args)... });
    }

private:
    EventServer() = default;

private:
    std::unordered_map<usize, std::unique_ptr<Handler>> m_handlers {};
};
