/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Assertion.hpp"

#include <memory>
#include <utility>

namespace HyperEngine
{
	template <class T, class... Args>
	auto construct_at(T* ptr, Args&&... args) -> T*
	{
		HYPERENGINE_ASSERT(ptr != nullptr);

		T* ptr = new (ptr) T(std::forward<Args>(args)...);
		return ptr;
	}

	template <class T>
	auto destroy_at(T* ptr) -> void
	{
		HYPERENGINE_ASSERT(ptr != nullptr);

		std::destroy_at(ptr);
	}
} // namespace HyperEngine
