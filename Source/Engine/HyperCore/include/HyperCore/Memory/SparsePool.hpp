/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Assertion.hpp"

#include <limits>
#include <memory>

namespace HyperCore
{
	template <class T, class Allocator = std::allocator<T>>
	class CSparsePool
	{
	public:
		using ValueType = T;

	private:
		using AllocatorTraits = std::allocator_traits<Allocator>;

	public:
		explicit CSparsePool(size_t size, Allocator allocator = Allocator())
			: m_allocator(allocator)
			, m_size(size)
			, m_data(AllocatorTraits::allocate(m_allocator, m_size))
		{
			HYPERENGINE_ASSERT(m_size > 1);

			for (size_t i = 0; i < m_size; ++i)
			{
				auto* address = reinterpret_cast<uint64_t*>(&m_data[i]);
				*address = static_cast<uint64_t>(i) + 1;
			}
		}

		~CSparsePool()
		{
			clear();
			AllocatorTraits::deallocate(m_allocator, m_data, m_size);
		}
		
		uint64_t push(const ValueType& value)
		{
			if (m_element_count >= m_size)
			{
				return std::numeric_limits<uint64_t>::max();
			}
			
			auto position = static_cast<uint64_t>(m_element_count);
			
			auto* address = reinterpret_cast<uint64_t*>(&m_data[position]);
			m_next_element = *address;
			
			m_data[position] = value;
			++m_element_count;
			
			return position;
		}
		
		uint64_t push(ValueType&& value)
		{
			if (m_element_count >= m_size)
			{
				return std::numeric_limits<uint64_t>::max();
			}
			
			auto position = static_cast<uint64_t>(m_element_count);
			
			auto* address = reinterpret_cast<uint64_t*>(&m_data[position]);
			m_next_element = *address;
			
			m_data[position] = std::move(value);
			++m_element_count;
			
			return position;
		}

		template <typename... Args>
		uint64_t emplace(Args&&... args)
		{
			if (m_element_count >= m_size)
			{
				return std::numeric_limits<uint64_t>::max();
			}
			
			auto position = static_cast<uint64_t>(m_element_count);
			
			auto* address = reinterpret_cast<uint64_t*>(&m_data[position]);
			m_next_element = *address;
			
			m_data[position] = ValueType(std::forward<Args>(args)...);
			++m_element_count;
			
			return position;
		}
		
		void remove(uint64_t position)
		{
			if (position >= m_size)
			{
				return;
			}

			auto* address = reinterpret_cast<uint64_t*>(&m_data[position]);
			*address = m_next_element;
			
			m_next_element = static_cast<uint64_t>(position);
			--m_element_count;

			m_data[position].~ValueType();
		}

		void clear()
		{
			for (size_t i = 0; i < m_size; ++i)
			{
				m_data[i].~ValueType();
				
				auto* address = reinterpret_cast<uint64_t*>(&m_data[i]);
				*address = static_cast<uint64_t>(i) + 1;
			}

			m_element_count = 0;
			m_next_element = 0;
		}

		ValueType& at(size_t position)
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_data[position];
		}

		const ValueType& at(size_t position) const
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_data[position];
		}
		[[nodiscard]]
		ValueType& operator[](size_t position)
		{
			return at(position);
		}

		const ValueType& operator[](size_t position) const
		{
			return at(position);
		}

		ValueType* data() noexcept
		{
			return m_data;
		}

		const ValueType* data() const noexcept
		{
			return m_data;
		}

		size_t size() const noexcept
		{
			return m_element_count;
		}

		size_t max_size() const noexcept
		{
			return m_size;
		}

		size_t capacity() const noexcept
		{
			return m_size;
		}

		bool empty() const noexcept
		{
			return size() == 0;
		}

	private:
		Allocator m_allocator{};
		
		size_t m_size{ 0 };
		ValueType* m_data{ nullptr };
		
		size_t m_element_count{ 0 };
		uint64_t m_next_element{ 0 };
	};
} // namespace HyperCore
