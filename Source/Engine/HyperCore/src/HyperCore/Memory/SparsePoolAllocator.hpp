/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Assertion.hpp>
#include <HyperCore/Logger.hpp>
#include <cstddef>
#include <cstdint>

namespace HyperCore
{
	template <typename T>
	class CSparsePoolAllocator
	{
	public:
		using ValueType = T;
		using Reference = T&;
		using ConstReference = const T&;
		using Pointer = T*;
		using ConstPointer = const T*;
		using SizeType = size_t;
		using DifferenceType = ptrdiff_t;
	
	public:
		explicit CSparsePoolAllocator(SizeType size)
			: m_size(size)
		{
			m_data = new ValueType[m_size];
			
			for (SizeType i = 0; i < m_size; ++i)
			{
				SizeType* address = reinterpret_cast<SizeType*>(&m_data[i]);
				*address = static_cast<SizeType>(i) + 1;
			}
		}
		
		~CSparsePoolAllocator()
		{
			delete[] m_data;
		}
		
		Reference allocate(SizeType& position)
		{
			HYPERENGINE_ASSERT(position < m_size);
			
			position = static_cast<SizeType>(m_allocation_count);
			
			SizeType* address = reinterpret_cast<SizeType*>(&m_data[position]);
			m_next_allocation = *address;
			
			m_data[position] = T();
			++m_allocation_count;
			
			return m_data[position];
		}
		
		void deallocate(SizeType position)
		{
			HYPERENGINE_ASSERT(position < m_size);
			
			SizeType* address = reinterpret_cast<SizeType*>(&m_data[position]);
			*address = m_next_allocation;
			
			m_next_allocation = position;
			
			m_data[position] = T();
			--m_allocation_count;
		}
		
		void clear()
		{
			for (SizeType i = 0; i < m_size; ++i)
			{
				SizeType* address = reinterpret_cast<SizeType*>(&m_data[i]);
				*address = static_cast<SizeType>(i) + 1;
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
		SizeType m_allocation_count{ 0 };
		SizeType m_next_allocation{ 0 };
	};
}