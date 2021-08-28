/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Memory/RingBuffer.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>

namespace HyperCore
{
	class JobSystem
	{
	public:
		struct DispatcherArgs
		{
			uint32_t job_index;
			uint32_t group_index;
		};

	public:
		JobSystem();
		~JobSystem();

		auto execute(const std::function<void()>& job) -> void;
		auto dispatch(uint32_t job_count, uint32_t group_size, const std::function<void(DispatcherArgs)>& job) -> void;

		auto busy() const -> bool;
		auto wait() -> void;

	private:
		uint32_t m_worker_count{ 0 };

		RingBuffer<std::function<void()>, 256> m_job_pool{};

		std::mutex m_wake_mutex{};
		std::condition_variable m_wake_condition{};

		uint64_t m_current_label{ 0 };
		std::atomic<uint64_t> m_finished_label{};

		std::atomic<bool> m_running_flag{};
		std::vector<std::thread> m_worker_threads;
	};
} // namespace HyperCore
