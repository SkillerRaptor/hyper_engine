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
		using Reference = ValueType&;
		using ConstReference = const ValueType&;
		using Pointer = ValueType*;
		using ConstPointer = const ValueType*;
		using SizeType = size_t;
		using DifferenceType = ptrdiff_t;

		using IndexType = I;

	public:
		explicit CSparsePoolAllocator(SizeType size = 128)
			: m_size(size)
		{
			m_data = new ValueType[m_size];

			for (SizeType i = 0; i < m_size; ++i)
			{
				IndexType* address = reinterpret_cast<IndexType*>(&m_data[i]);
				*address = static_cast<IndexType>(i) + 1;
			}
		}

		~CSparsePoolAllocator()
		{
			delete[] m_data;
		}

		Reference allocate(IndexType& position)
		{
			position = static_cast<SizeType>(m_allocation_count);

			auto* address = reinterpret_cast<IndexType*>(&m_data[position]);
			m_next_allocation = *address;

			m_data[position] = T();
			++m_allocation_count;

			return m_data[position];
		}

		bool deallocate(IndexType& position)
		{
			if(position >= m_size)
			{
				HyperCore::CLogger::warning("HyperCore: SparsePoolAllocator: position out of range!");
				return false;
			}

			auto* address = reinterpret_cast<IndexType*>(&m_data[position]);
			*address = m_next_allocation;

			m_next_allocation = static_cast<IndexType>(position);

			m_data[position] = T();
			--m_allocation_count;
			
			position = std::numeric_limits<IndexType>::max();
			
			return true;
		}

		void clear()
		{
			for (SizeType i = 0; i < m_size; ++i)
			{
				IndexType* address = reinterpret_cast<IndexType*>(&m_data[i]);
				*address = static_cast<IndexType>(i) + 1;
			}

			m_allocation_count = 0;
			m_next_allocation = 0;
		}

		constexpr Reference at(SizeType position)
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_data[position];
		}

		constexpr ConstReference at(SizeType position) const
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_data[position];
		}

		constexpr Reference operator[](SizeType position)
		{
			return m_data[position];
		}

		constexpr ConstReference operator[](SizeType position) const
		{
			return m_data[position];
		}

		constexpr Pointer data() noexcept
		{
			return m_data;
		}

		constexpr ConstPointer data() const noexcept
		{
			return m_data;
		}

		constexpr bool empty() const noexcept
		{
			return size() == 0;
		}

		constexpr SizeType size() const noexcept
		{
			return m_allocation_count;
		}

		constexpr SizeType max_size() const noexcept
		{
			return m_size;
		}

	private:
		Pointer m_data{ nullptr };
		SizeType m_size{ 0 };

		IndexType m_allocation_count{ 0 };
		IndexType m_next_allocation{ 0 };
	};
} // namespace HyperCore
