/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperCore
{
	template <typename T>
	struct IsDataStruct
	{
		static constexpr bool value =
			!std::is_const_v<T> &&
			!std::is_volatile_v<T> &&
			!std::is_reference_v<T> &&
			!std::is_pointer_v<T>;
	};
} // namespace HyperCore
