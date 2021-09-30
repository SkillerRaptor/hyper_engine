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
	constexpr auto bit(const T shift) noexcept -> T
	{
		static_assert(std::is_unsigned_v<T>, "'T' is not an unsigned integral value!");

		HYPERENGINE_ASSERT_IS_NOT_EQUAL(shift, 0, "The value of 'shift' is 0");

		return static_cast<T>(1) << shift;
	}

	template <typename T>
	constexpr auto mask(const T shift) noexcept -> T
	{
		static_assert(std::is_unsigned_v<T>, "'T' is not an unsigned integral value!");

		HYPERENGINE_ASSERT_IS_NOT_EQUAL(shift, 0, "The value of 'shift' is 0");

		return bit<T>(shift) - static_cast<T>(1);
	}
} // namespace HyperEngine
