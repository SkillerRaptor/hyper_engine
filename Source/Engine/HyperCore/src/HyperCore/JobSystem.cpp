/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/JobSystem.hpp"

#include "HyperCore/Logger.hpp"

#include <algorithm>

namespace HyperEngine
{
	size_t JobSystem::s_worker_count{ 0 };

	RingBuffer<std::function<void()>, 256> JobSystem::s_job_pool{};

	std::mutex JobSystem::s_wake_mutex{};
	std::condition_variable JobSystem::s_wake_condition{};

	size_t JobSystem::s_current_label{ 0 };
	std::atomic<uint64_t> JobSystem::s_finished_label{};

	std::atomic<bool> JobSystem::s_running_flag{};
	std::vector<std::thread> JobSystem::s_worker_threads{};

	auto JobSystem::initialize() -> bool
	{
		s_finished_label.store(0);
		s_running_flag.store(true);

		const size_t core_count = std::thread::hardware_concurrency();
		s_worker_count = std::max(static_cast<size_t>(1), core_count);

		for (size_t i = 0; i < s_worker_count; ++i)
		{
			Logger::debug("Starting worker thread #{}", i);

			std::thread worker(
				[]()
				{
					std::function<void()> job;

					while (s_running_flag.load())
					{
						if (!s_job_pool.pop_front(job))
						{
							std::unique_lock<std::mutex> lock(s_wake_mutex);
							s_wake_condition.wait(lock);
							continue;
						}

						job();
						s_finished_label.fetch_add(1);
					}
				});

			s_worker_threads.push_back(std::move(worker));
		}

		Logger::debug("JobSystem was successfully initialized");

		return true;
	}

	auto JobSystem::terminate() -> void
	{
		s_running_flag.store(false);
		s_wake_condition.notify_all();

		size_t worker_id = 0;
		for (std::thread& worker_thread : s_worker_threads)
		{
			Logger::debug("Stopping worker thread #{}", worker_id);

			worker_thread.join();
			++worker_id;
		}

		Logger::debug("JobSystem was successfully terminated");
	}

	auto JobSystem::execute(const std::function<void()>& job) -> void
	{
		s_current_label += 1;
		while (!s_job_pool.push_back(job))
		{
			s_wake_condition.notify_one();
			std::this_thread::yield();
		}

		s_wake_condition.notify_one();

		Logger::debug("Executing job with label id '{}'", s_current_label);
	}

	auto JobSystem::dispatch(const size_t job_count, const size_t group_size, const std::function<void(DispatcherArgs)>& job) -> void
	{
		if (job_count == 0)
		{
			Logger::fatal("JobSystem::dispatch(): Failed to dispatch job with a job count of 0");
			return;
		}

		if (group_size == 0)
		{
			Logger::fatal("JobSystem::dispatch(): Failed to dispatch job with a group count of 0");
			return;
		}

		const size_t group_count = (job_count + group_size - 1) / group_size;
		s_current_label += group_count;

		for (size_t group_index = 0; group_index < group_count; ++group_index)
		{
			auto job_function = [group_index, group_size, job_count, job]()
			{
				const size_t group_job_offset = group_index * group_size;
				const size_t group_job_end = std::min(group_job_offset + group_size, job_count);

				DispatcherArgs dispatcher_args{};
				dispatcher_args.group_index = group_index;

				for (size_t i = group_job_offset; i < group_job_end; ++i)
				{
					dispatcher_args.job_index = i;
					job(dispatcher_args);
				}
			};

			while (!s_job_pool.push_back(job_function))
			{
				s_wake_condition.notify_one();
				std::this_thread::yield();
			}

			s_wake_condition.notify_one();

			Logger::debug("Executing job with label id '{}'", s_current_label - group_index);
		}
	}

	auto JobSystem::busy() -> bool
	{
		return s_finished_label.load() < s_current_label;
	}

	auto JobSystem::wait() -> void
	{
		while (busy())
		{
			s_wake_condition.notify_one();
			std::this_thread::yield();
		}
	}
} // namespace HyperEngine
