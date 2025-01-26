/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mutex>

namespace hyper_engine
{
    template <typename T, size_t N>
    class ThreadSafeRingBuffer
    {
    public:
        bool push_back(const T &item)
        {
            bool result = false;
            m_lock.lock();

            const size_t next = (m_head + 1) % N;
            if (next != m_tail)
            {
                m_data[m_head] = item;
                m_head = next;
                result = true;
            }

            m_lock.unlock();
            return result;
        }

        bool pop_front(T &item)
        {
            bool result = false;
            m_lock.lock();

            if (m_tail != m_head)
            {
                item = m_data[m_tail];
                m_tail = (m_tail + 1) % N;
                result = true;
            }

            m_lock.unlock();
            return result;
        }

    private:
        T m_data[N];
        size_t m_head = 0;
        size_t m_tail = 0;
        std::mutex m_lock;
    };
} // namespace hyper_engine