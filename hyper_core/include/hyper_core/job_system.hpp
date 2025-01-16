/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

// NOTE: Based on the implementation of https://wickedengine.net/2018/11/simple-job-system-using-standard-c

#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>

#include "hyper_core/thread_safe_ring_buffer.hpp"

namespace hyper_engine
{
    struct DispatchArgs
    {
        uint32_t job_index = 0;
        uint32_t group_index = 0;
    };

    class JobSystem
    {
    public:
        JobSystem();

        void execute(const std::function<void()> &job);
        void dispatch(uint32_t job_count, uint32_t group_size, const std::function<void(DispatchArgs)> &job);

        bool is_busy() const;

        void wait_for_idle();

    private:
        uint32_t m_thread_count = 0;
        ThreadSafeRingBuffer<std::function<void()>, 256> m_job_pool;
        std::condition_variable m_wake_condition;
        std::mutex m_wake_mutex;
        uint64_t m_current_label = 0;
        std::atomic<uint64_t> m_finished_label;
    };
} // namespace hyper_engine