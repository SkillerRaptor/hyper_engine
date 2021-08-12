/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Assertion.hpp"

#include <memory>

namespace HyperCore
{
	/*
	 * TODO: Rework RingBuffer
	 */
	
	template <class T, class Allocator = std::allocator<T>>
	class CRingBuffer
	{
	public:
		using ValueType = T;

	private:
		using AllocatorTraits = std::allocator_traits<Allocator>;

	public:
		explicit CRingBuffer(size_t size, Allocator allocator = Allocator())
			: m_allocator(allocator)
			, m_size(size)
			, m_data(AllocatorTraits::allocate(m_allocator, m_size))
		{
			HYPERENGINE_ASSERT(m_size > 1);
		}

		~CRingBuffer()
		{
			clear();
			AllocatorTraits::deallocate(m_allocator, m_data, m_size);
		}

		void push_back(const ValueType& value)
		{
			if (m_element_count >= m_size)
			{
				return;
			}

			size_t index = (m_element_count + m_next_element) % m_size;
			m_data[index] = value;
			++m_element_count;
		}

		void push(const ValueType& value)
		{
			push_back(value);
		}

		void push_back(ValueType&& value)
		{
			if (m_element_count >= m_size)
			{
				return;
			}

			size_t index = (m_element_count + m_next_element) % m_size;
			m_data[index] = std::move(value);
			++m_element_count;
		}

		void push(ValueType&& value)
		{
			push_back(std::move(value));
		}

		template <typename... Args>
		void emplace_back(Args&&... args)
		{
			if (m_element_count >= m_size)
			{
				return;
			}

			size_t index = (m_element_count + m_next_element) % m_size;
			m_data[index] = ValueType(std::forward<Args>(args)...);
			++m_element_count;
		}

		template <typename... Args>
		void emplace(Args&&... args)
		{
			emplace_back(std::forward<Args>(args)...);
		}

		void pop_back()
		{
			if (m_element_count <= 0)
			{
				return;
			}

			size_t index = m_next_element;
			m_next_element = (m_next_element + 1) % m_size;
			--m_element_count;

			m_data[index].~ValueType();
		}

		void pop()
		{
			pop_back();
		}

		void clear()
		{
			for (size_t i = 0; i < m_element_count; ++i)
			{
				m_data[i].~ValueType();
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

		ValueType& operator[](size_t position)
		{
			return at(position);
		}

		const ValueType& operator[](size_t position) const
		{
			return at(position);
		}

		ValueType* data()
		{
			return m_data;
		}

		const ValueType* data() const
		{
			return m_data;
		}

		size_t size() const
		{
			return m_size;
		}

		size_t max_size() const
		{
			return m_size;
		}

		size_t capacity() const
		{
			return m_size;
		}

		bool empty() const
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
