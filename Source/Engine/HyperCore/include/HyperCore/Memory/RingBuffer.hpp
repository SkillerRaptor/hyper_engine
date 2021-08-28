/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mutex>

namespace HyperCore
{
	template <typename T, size_t S>
	class RingBuffer
	{
	public:
		auto push_back(const T& item) -> bool
		{
			m_lock.lock();

			bool result = false;
			size_t next = (m_head + 1) % S;
			if (next != m_tail)
			{
				m_data[m_head] = item;
				m_head = next;
				result = true;
			}

			m_lock.unlock();
			return result;
		}

		auto pop_front(T& item) -> bool
		{
			m_lock.lock();

			bool result = false;
			if (m_tail != m_head)
			{
				item = m_data[m_tail];
				m_tail = (m_tail + 1) % S;
				result = true;
			}

			m_lock.unlock();
			return result;
		}

	private:
		T m_data[S]{};

		size_t m_head{ 0 };
		size_t m_tail{ 0 };

		std::mutex m_lock;
	};
} // namespace HyperCore
