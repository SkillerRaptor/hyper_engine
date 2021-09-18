/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Prerequisites.hpp"
#include "HyperCore/Containers/RingBuffer.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>

namespace HyperEngine
{
	class JobSystem
	{
	public:
		HYPERENGINE_MAKE_SINGLETON(JobSystem);

	public:
		struct DispatcherArgs
		{
			size_t job_index;
			size_t group_index;
		};

	public:
		static auto initialize() -> bool;
		static auto terminate() -> void;

		static auto execute(const std::function<void()>& job) -> void;
		static auto dispatch(const size_t job_count, const size_t group_size, const std::function<void(DispatcherArgs)>& job) -> void;

		static auto busy() -> bool;
		static auto wait() -> void;

	private:
		static size_t s_worker_count;

		static RingBuffer<std::function<void()>, 256> s_job_pool;

		static std::mutex s_wake_mutex;
		static std::condition_variable s_wake_condition;

		static size_t s_current_label;
		static std::atomic<uint64_t> s_finished_label;

		static std::atomic<bool> s_running_flag;
		static std::vector<std::thread> s_worker_threads;
	};
} // namespace HyperEngine
