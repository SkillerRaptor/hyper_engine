/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Assertion.hpp"

#include <cstddef>
#include <type_traits>

namespace HyperEngine
{
	template <class T>
	constexpr auto align(const T n, const size_t align) noexcept -> T
	{
		static_assert(std::is_unsigned_v<T>, "'T' is not an unsigned integral value!");

		HYPERENGINE_ASSERT((align & (align - 1)) == 0);

		const size_t size = (static_cast<size_t>(n) + (align - 1)) & ~(align - 1);
		return static_cast<T>(size);
	}

	template <class T>
	constexpr auto aligned(const T n, const size_t align) noexcept -> bool
	{
		static_assert(std::is_unsigned_v<T>, "'T' is not an unsigned integral value!");

		HYPERENGINE_ASSERT((align & (align - 1)) == 0);

		const size_t size = static_cast<size_t>(n) & (align - 1);
		return size == 0;
	}
} // namespace HyperEngine
