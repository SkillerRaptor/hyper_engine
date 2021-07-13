/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Assertion.hpp>
#include <HyperCore/Logger.hpp>

namespace HyperCore
{
	template <typename T, typename I = uint32_t>
	class CSparsePoolAllocator
	{
	public:
		using ValueType = T;
		using IndexType = I;
		
	public:
		explicit CSparsePoolAllocator(size_t size = 128)
			: m_size(size)
		{
			m_data = new T[m_size];

			for (size_t i = 0; i < m_size; ++i)
			{
				I* address = reinterpret_cast<I*>(&m_data[i]);
				*address = static_cast<I>(i) + 1;
			}
		}

		~CSparsePoolAllocator()
		{
			delete[] m_data;
		}

		T& allocate(I& position)
		{
			position = static_cast<size_t>(m_allocation_count);

			auto* address = reinterpret_cast<I*>(&m_data[position]);
			m_next_allocation = *address;

			m_data[position] = T();
			++m_allocation_count;

			return m_data[position];
		}

		bool deallocate(I position)
		{
			if(position >= m_size)
			{
				HyperCore::CLogger::warning("CSparsePoolAllocator: position out of range!");
				return false;
			}

			auto* address = reinterpret_cast<I*>(&m_data[position]);
			*address = m_next_allocation;

			m_next_allocation = static_cast<I>(position);

			m_data[position] = T();
			--m_allocation_count;
			
			return true;
		}

		void clear()
		{
			for (size_t i = 0; i < m_size; ++i)
			{
				I* address = reinterpret_cast<I*>(&m_data[i]);
				*address = static_cast<I>(i) + 1;
			}

			m_allocation_count = 0;
			m_next_allocation = 0;
		}

		constexpr T& at(size_t position)
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_data[position];
		}

		constexpr const T& at(size_t position) const
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_data[position];
		}

		constexpr T& operator[](size_t position)
		{
			return m_data[position];
		}

		constexpr const T& operator[](size_t position) const
		{
			return m_data[position];
		}

		constexpr T* data() noexcept
		{
			return m_data;
		}

		constexpr const T* data() const noexcept
		{
			return m_data;
		}

		constexpr bool empty() const noexcept
		{
			return size() == 0;
		}

		constexpr size_t size() const noexcept
		{
			return m_allocation_count;
		}

		constexpr size_t max_size() const noexcept
		{
			return m_size;
		}

	private:
		T* m_data{ nullptr };
		size_t m_size{ 0 };

		I m_allocation_count{ 0 };
		I m_next_allocation{ 0 };
	};
} // namespace HyperCore
