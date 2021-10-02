/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/Assertion.hpp"

#include <cstddef>
#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	[[nodiscard]] constexpr auto align(const T value, const size_t align) noexcept -> T
	{
		static_assert(std::is_unsigned_v<T>, "'T' is not an unsigned integral value!");

		HYPERENGINE_ASSERT_IS_EQUAL(align & (align - 1), 0);

		const size_t size = (static_cast<size_t>(value) + (align - 1)) & ~(align - 1);
		return static_cast<T>(size);
	}

	template <typename T>
	[[nodiscard]] constexpr auto is_aligned(const T value, const size_t align) noexcept -> bool
	{
		static_assert(std::is_unsigned_v<T>, "'T' is not an unsigned integral value!");

		HYPERENGINE_ASSERT_IS_EQUAL(align & (align - 1), 0);

		const size_t size = static_cast<size_t>(value) & (align - 1);
		return size == 0;
	}
} // namespace HyperEngine
