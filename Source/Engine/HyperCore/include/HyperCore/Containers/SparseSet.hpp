/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Assertion.hpp>
#include <vector>

namespace HyperCore
{
	template <typename T>
	class CSparseSet
	{
	public:
		using ValueType = T;
		
	public:
		~CSparseSet()
		{
			clear();
		}

		void clear() noexcept
		{
			m_sparse.clear();
			m_packed.clear();
		}

		T& at(size_t position)
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_packed[position];
		}

		const T& at(size_t position) const
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_packed[position];
		}

		T& operator[](size_t position)
		{
			return m_packed[position];
		}

		const T& operator[](size_t position) const
		{
			return m_packed[position];
		}

		T* data() noexcept
		{
			return m_data;
		}

		const T* data() const noexcept
		{
			return m_data;
		}

		size_t size() const noexcept
		{
			return m_size;
		}

		size_t max_size() const noexcept
		{
			return m_size;
		}

		bool empty() const noexcept
		{
			return size() == 0;
		}

	private:
		std::vector<T> m_sparse;
		std::vector<T> m_packed;
		size_t m_size;
		size_t m_capacity;
	};
} // namespace HyperCore
