/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/JobSystem.hpp"

#include "HyperCore/Logger.hpp"

#include <algorithm>

namespace HyperCore
{
	JobSystem::JobSystem()
	{
		m_finished_label.store(0);
		m_running_flag.store(true);

		auto core_count = std::thread::hardware_concurrency();
		m_worker_count = std::max(1U, core_count);

		for (uint32_t worker_id = 0; worker_id < m_worker_count; ++worker_id)
		{
			std::thread worker(
				[this]()
				{
					std::function<void()> job;

					while (m_running_flag.load())
					{
						if (!m_job_pool.pop_front(job))
						{
							std::unique_lock<std::mutex> lock(m_wake_mutex);
							m_wake_condition.wait(lock);
							continue;
						}

						job();
						m_finished_label.fetch_add(1);
					}
				});

			m_worker_threads.push_back(std::move(worker));
		}
	}

	JobSystem::~JobSystem()
	{
		m_running_flag.store(false);
		m_wake_condition.notify_all();

		for (auto& worker_thread : m_worker_threads)
		{
			worker_thread.join();
		}
	}

	auto JobSystem::execute(const std::function<void()>& job) -> void
	{
		m_current_label += 1;
		while (!m_job_pool.push_back(job))
		{
			m_wake_condition.notify_one();
			std::this_thread::yield();
		}

		m_wake_condition.notify_one();
	}

	auto JobSystem::dispatch(uint32_t job_count, uint32_t group_size, const std::function<void(DispatcherArgs)>& job) -> void
	{
		if (job_count == 0)
		{
			return;
		}

		if (group_size == 0)
		{
			return;
		}

		const uint32_t group_count = (job_count + group_size - 1) / group_size;
		m_current_label += group_count;
	
		for (uint32_t group_index = 0; group_index < group_count; ++group_index)
		{
			auto job_function = [&]()
			{
				const uint32_t group_job_offset = group_index * group_size;
				const uint32_t group_job_end = std::min(group_job_offset + group_size, job_count);
 
				DispatcherArgs dispatcher_args{};
				dispatcher_args.group_index = group_index;
				
				for (uint32_t i = group_job_offset; i < group_job_end; ++i)
				{
					dispatcher_args.job_index = i;
					job(dispatcher_args);
				}
			};
			
			while (!m_job_pool.push_back(job_function))
			{
				m_wake_condition.notify_one();
				std::this_thread::yield();
			}
			
			m_wake_condition.notify_one();
		}
	}

	auto JobSystem::busy() const -> bool
	{
		return m_finished_label.load() < m_current_label;
	}

	auto JobSystem::wait() -> void
	{
		while (busy())
		{
			m_wake_condition.notify_one();
			std::this_thread::yield();
		}
	}
} // namespace HyperCore
