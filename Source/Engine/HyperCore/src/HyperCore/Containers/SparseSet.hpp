/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
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
		using Reference = ValueType&;
		using ConstReference = const ValueType&;
		using Pointer = ValueType*;
		using ConstPointer = const ValueType*;
		using SizeType = size_t;
		using DifferenceType = ptrdiff_t;

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

		constexpr Reference at(SizeType position)
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_packed[position];
		}

		constexpr ConstReference at(SizeType position) const
		{
			HYPERENGINE_ASSERT(position < m_size);
			return m_packed[position];
		}

		constexpr Reference operator[](SizeType position)
		{
			return m_packed[position];
		}

		constexpr ConstReference operator[](SizeType position) const
		{
			return m_packed[position];
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
			return m_size;
		}

		constexpr SizeType max_size() const noexcept
		{
			return m_size;
		}

	private:
		std::vector<ValueType> m_sparse;
		std::vector<ValueType> m_packed;
		SizeType m_size;
		SizeType m_capacity;
	};
} // namespace HyperCore
