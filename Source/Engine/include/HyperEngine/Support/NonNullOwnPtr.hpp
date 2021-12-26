/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Concepts.hpp"

#include <cassert>
#include <cstddef>
#include <functional>
#include <utility>

namespace HyperEngine
{
	template <typename T>
	class NonNullOwnPtr
	{
	public:
		using PointerT = typename std::remove_pointer_t<T> *;
		using ConstPointerT = const typename std::remove_pointer_t<T> *;
		using RefT = typename std::remove_pointer_t<T> &;
		using ConstRefT = const typename std::remove_pointer_t<T> &;
		using DeleterT = std::function<void(PointerT)>;

	public:
		NonNullOwnPtr() = default;

		NonNullOwnPtr(RefT ptr) requires(!Pointer<T>)
			: m_ptr(&ptr)
		{
		}

		NonNullOwnPtr(RefT ptr, const DeleterT &deleter) requires(!Pointer<T>)
			: m_ptr(&ptr)
			, m_deleter(deleter)
		{
		}

		NonNullOwnPtr(PointerT ptr) requires(Pointer<T>)
			: m_ptr(ptr)
		{
		}

		NonNullOwnPtr(PointerT ptr, const DeleterT &deleter) requires(Pointer<T>)
			: m_ptr(ptr)
			, m_deleter(deleter)
		{
		}

		NonNullOwnPtr(std::nullptr_t) = delete;

		~NonNullOwnPtr()
		{
			reset();
		}

		NonNullOwnPtr(const NonNullOwnPtr &) = delete;
		NonNullOwnPtr &operator=(const NonNullOwnPtr &) = delete;

		NonNullOwnPtr(NonNullOwnPtr &&other) noexcept
			: m_ptr(std::exchange(other.m_ptr, nullptr))
		{
		}

		NonNullOwnPtr &operator=(NonNullOwnPtr &&other) noexcept
		{
			m_ptr = std::exchange(other.m_ptr, nullptr);
			return *this;
		}

		NonNullOwnPtr &operator=(PointerT ptr) = delete;

		PointerT release() noexcept
		{
			return std::exchange(m_ptr, nullptr);
		}

		PointerT ptr() const noexcept
		{
			assert(m_ptr != nullptr && "The ptr can't be null");
			return m_ptr;
		}

		PointerT operator->()
		{
			assert(m_ptr != nullptr && "The ptr can't be null");
			return m_ptr;
		}

		ConstPointerT operator->() const
		{
			assert(m_ptr != nullptr && "The ptr can't be null");
			return m_ptr;
		}

		RefT operator*()
		{
			assert(m_ptr != nullptr && "The ptr can't be null");
			return *m_ptr;
		}

		ConstRefT operator*() const
		{
			assert(m_ptr != nullptr && "The ptr can't be null");
			return *m_ptr;
		}

		operator PointerT() const noexcept
		{
			assert(m_ptr != nullptr && "The ptr can't be null");
			return m_ptr;
		}

		operator bool() const noexcept = delete;
		bool operator!() const noexcept = delete;

	private:
		void reset()
		{
			if (m_ptr == nullptr)
			{
				return;
			}

			if (m_deleter)
			{
				m_deleter(m_ptr);
				return;
			}

			if constexpr (!std::is_pointer_v<T>)
			{
				delete m_ptr;
			}
		}

	private:
		PointerT m_ptr = nullptr;
		DeleterT m_deleter = nullptr;
	};

	template <typename T, typename... Args>
	requires Constructible<T, Args...>
	static NonNullOwnPtr<T> make_non_null_own(Args &&...args)
	{
		return *new T(std::forward<Args>(args)...);
	}
} // namespace HyperEngine
