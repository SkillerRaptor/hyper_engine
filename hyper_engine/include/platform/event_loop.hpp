/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>

#include "core/memory.hpp"
#include "core/types.hpp"

namespace he
{
    class EventLoop
    {
    private:
        template <typename T>
        using Callback = std::function<void(const T &)>;

        class IdGenerator
        {
        public:
            template <typename>
            static u32 type()
            {
                static const u32 value = identifier();
                return value;
            }

        private:
            static u32 identifier()
            {
                static u32 value { 0 };
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
        public:
            void add_listener(const Callback<T> &callback) { m_callbacks.push_back(callback); }

            void dispatch(const T &event) const
            {
                for (const Callback<T> &callback : m_callbacks)
                {
                    callback(event);
                }
            }

        private:
            std::vector<Callback<T>> m_callbacks {};
        };

    public:
        EventLoop() = default;
        ~EventLoop() = default;

        EventLoop(const EventLoop &) = delete;
        EventLoop &operator=(const EventLoop &) = delete;

        EventLoop(EventLoop &&) noexcept = default;
        EventLoop &operator=(EventLoop &&) noexcept = default;

        void poll() const;

        template <typename T>
        void add_listener(const Callback<T> &callback)
        {
            const u32 id = IdGenerator::type<T>();
            if (!m_handlers.contains(id))
            {
                m_handlers.insert({ id, std::make_unique<HandlerImpl<T>>() });
            }

            HandlerImpl<T> *handler = static_cast<HandlerImpl<T> *>(m_handlers.at(id).get());
            handler->add_listener(callback);
        }

    private:
        template <typename T, typename... Args>
        void dispatch(Args &&...args) const
        {
            const u32 id = IdGenerator::type<T>();
            if (!m_handlers.contains(id))
            {
                return;
            }

            const HandlerImpl<T> *handler = static_cast<HandlerImpl<T> *>(m_handlers.at(id).get());
            handler->dispatch(T { std::forward<Args>(args)... });
        }

    private:
        std::unordered_map<u32, OwnPtr<Handler>> m_handlers {};
    };
} // namespace he
