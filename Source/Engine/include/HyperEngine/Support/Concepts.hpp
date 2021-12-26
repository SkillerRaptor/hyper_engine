/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <ostream>
#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;

	template <typename T, typename... Args>
	concept Constructible = std::constructible_from<T, Args...>;

	template <typename DerivedT, typename BaseT>
	concept Derived = std::derived_from<DerivedT, BaseT>;

	template <typename T>
	concept Pointer = std::is_pointer_v<T>;

	template <typename T>
	concept Printable = requires(std::ostream &ostream, T value)
	{
		ostream << value;
	};
} // namespace HyperEngine
