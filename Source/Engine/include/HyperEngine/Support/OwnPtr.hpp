/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Concepts.hpp"
#include "HyperEngine/Support/NonNullOwnPtr.hpp"

#include <cassert>
#include <cstddef>
#include <functional>
#include <utility>

namespace HyperEngine
{
	template <typename T>
	class OwnPtr
	{
	public:
		using PointerT = typename std::remove_pointer_t<T> *;
		using ConstPointerT = const typename std::remove_pointer_t<T> *;
		using RefT = typename std::remove_pointer_t<T> &;
		using ConstRefT = const typename std::remove_pointer_t<T> &;
		using DeleterT = std::function<void(PointerT)>;

	public:
		OwnPtr() = default;

		OwnPtr(RefT ptr) requires(!Pointer<T>)
			: m_ptr(&ptr)
		{
		}

		OwnPtr(RefT ptr, const DeleterT &deleter) requires(!Pointer<T>)
			: m_ptr(&ptr)
			, m_deleter(deleter)
		{
		}

		OwnPtr(PointerT ptr) requires(Pointer<T>)
			: m_ptr(ptr)
		{
		}

		OwnPtr(PointerT ptr, const DeleterT &deleter) requires(Pointer<T>)
			: m_ptr(ptr)
			, m_deleter(deleter)
		{
		}

		OwnPtr(std::nullptr_t)
			: m_ptr(nullptr)
		{
		}

		~OwnPtr()
		{
			reset();
		}

		OwnPtr(const OwnPtr &) = delete;
		OwnPtr &operator=(const OwnPtr &) = delete;

		OwnPtr(OwnPtr &&other) noexcept
			: m_ptr(std::exchange(other.m_ptr, nullptr))
		{
		}

		OwnPtr &operator=(OwnPtr &&other) noexcept
		{
			m_ptr = std::exchange(other.m_ptr, nullptr);
			return *this;
		}

		OwnPtr &operator=(NonNullOwnPtr<T> &&ptr)
		{
			reset();
			m_ptr = ptr.release();
			return *this;
		}

		OwnPtr &operator=(PointerT ptr)
		{
			reset();
			m_ptr = std::exchange(ptr, nullptr);
			return *this;
		}

		OwnPtr &operator=(std::nullptr_t)
		{
			reset();
			return *this;
		}

		PointerT release() noexcept
		{
			return std::exchange(m_ptr, nullptr);
		}

		NonNullOwnPtr<T> release_non_null()
		{
			assert(m_ptr != nullptr && "The ptr can't be null");
			if constexpr (Pointer<T>)
			{
				return NonNullOwnPtr<T>(release());
			}
			else
			{
				return NonNullOwnPtr<T>(*release());
			}
		}

		PointerT ptr() const noexcept
		{
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
			return m_ptr;
		}

		operator bool() const noexcept
		{
			return m_ptr == nullptr;
		}

		bool operator!() const noexcept
		{
			return m_ptr != nullptr;
		}

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
	static OwnPtr<T> make_own(Args &&...args)
	{
		return new T(std::forward<Args>(args)...);
	}
} // namespace HyperEngine
