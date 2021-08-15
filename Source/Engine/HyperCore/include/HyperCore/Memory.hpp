/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <functional>
#include <memory>

namespace HyperCore
{
	template <typename T, typename... Args>
	static auto allocate(Args&&... args) -> T*
	{
		void* ptr = malloc(sizeof(T));
		new (ptr) T(std::forward<Args>(args)...);
		return static_cast<T*>(ptr);
	}

	template <typename T>
	static auto deallocate(T* ptr) -> void
	{
		if (ptr == nullptr)
		{
			return;
		}

		ptr->~T();
		free(ptr);
	}
} // namespace 
