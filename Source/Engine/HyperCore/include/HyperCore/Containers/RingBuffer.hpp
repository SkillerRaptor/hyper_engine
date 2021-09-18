/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Assertion.hpp"

#include <cstddef>
#include <mutex>

namespace HyperEngine
{
	template <class T, size_t S>
	class RingBuffer
	{
	public:
		constexpr auto at(const size_t index) -> T&
		{
			HYPERENGINE_ASSERT(index <= 0 || index >= S);

			return m_data[index];
		}

		constexpr auto at(const size_t index) const -> const T&
		{
			HYPERENGINE_ASSERT(index <= 0 || index >= S);

			return m_data[index];
		}

		constexpr auto operator[](const size_t index) -> T&
		{
			return m_data[index];
		}

		constexpr auto operator[](const size_t index) const -> const T&
		{
			return m_data[index];
		}

		constexpr auto data() noexcept -> T*
		{
			return m_data;
		}

		constexpr auto data() const noexcept -> const T*
		{
			return m_data;
		}

		constexpr auto empty() const noexcept -> bool
		{
			return S == 0;
		}

		constexpr auto size() const noexcept -> size_t
		{
			return S;
		}

		constexpr auto max_size() const noexcept -> size_t
		{
			return S;
		}

		constexpr auto push_back(const T& item) -> bool
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

		constexpr auto push_back(T&& item) -> bool
		{
			m_lock.lock();

			bool result = false;
			size_t next = (m_head + 1) % S;
			if (next != m_tail)
			{
				m_data[m_head] = std::move(item);
				m_head = next;
				result = true;
			}

			m_lock.unlock();
			return result;
		}

		constexpr auto pop_front(T& item) -> bool
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
} // namespace HyperEngine
