/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/job_system.hpp"

#include "hyper_core/logger.hpp"

#include <algorithm>
#include <thread>

namespace hyper_engine
{
    JobSystem::JobSystem()
    {
        m_finished_label.store(0);

        const uint32_t max_threads = std::thread::hardware_concurrency();
        m_thread_count = std::max(1u, max_threads);

        for (uint32_t thread_id = 0; thread_id < m_thread_count; ++thread_id)
        {
            std::thread worker_thread(
                [this]()
                {
                    std::function<void()> job;

                    while (true)
                    {
                        if (m_job_pool.pop_front(job))
                        {
                            job();
                            m_finished_label.fetch_add(1);
                        }
                        else
                        {
                            std::unique_lock<std::mutex> lock(m_wake_mutex);
                            m_wake_condition.wait(lock);
                        }
                    }
                });

            worker_thread.detach();
        }
    }

    void JobSystem::execute(const std::function<void()> &job)
    {
        m_current_label += 1;

        while (!m_job_pool.push_back(job))
        {
            m_wake_condition.notify_one();
            std::this_thread::yield();
        }

        m_wake_condition.notify_one();
    }

    void JobSystem::dispatch(const uint32_t job_count, const uint32_t group_size, const std::function<void(DispatchArgs)> &job)
    {
        if (job_count == 0 || group_size == 0)
        {
            return;
        }

        const uint32_t group_count = (job_count + group_size - 1) / group_size;
        m_current_label += group_count;

        for (uint32_t group_index = 0; group_index < group_count; ++group_index)
        {
            auto job_group = [job_count, group_size, job, group_index]()
            {
                const uint32_t group_job_offset = group_index * group_size;
                const uint32_t group_job_end = std::min(group_job_offset + group_size, job_count);

                DispatchArgs args = {};
                args.group_index = group_index;

                for (uint32_t i = group_job_offset; i < group_job_end; ++i)
                {
                    args.job_index = i;
                    job(args);
                }
            };

            while (!m_job_pool.push_back(job_group))
            {
                m_wake_condition.notify_one();
                std::this_thread::yield();
            }

            m_wake_condition.notify_one();
        }
    }

    bool JobSystem::is_busy() const
    {
        return m_finished_label.load() < m_current_label;
    }

    void JobSystem::wait_for_idle()
    {
        while (is_busy())
        {
            m_wake_condition.notify_one();
            std::this_thread::yield();
        }
    }
} // namespace hyper_engine