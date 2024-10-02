/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>
#include <vector>

namespace hyper_event
{
    class EventHandler
    {
    public:
        virtual ~EventHandler() = default;
    };

    template <typename T>
    class EventHandlerImpl : public EventHandler
    {
    public:
        void subscribe(const std::function<void(const T &)> &callback)
        {
            m_callbacks.push_back(callback);
        }

        void dispatch(const T &event)
        {
            for (const std::function<void(const T &)> &callback : m_callbacks)
            {
                callback(event);
            }
        }

    private:
        std::vector<std::function<void(const T &)>> m_callbacks;
    };
} // namespace hyper_event
