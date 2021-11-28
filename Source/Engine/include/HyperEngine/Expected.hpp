/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Error.hpp"

#include <cassert>
#include <optional>

namespace HyperEngine
{
	template <typename T, typename ErrorT = Error>
	class Expected
	{
	public:
		Expected(const T &value)
			: m_value(value)
		{
		}

		Expected(T &&value)
			: m_value(std::move(value))
		{
		}

		Expected(const ErrorT &error)
			: m_error(error)
		{
		}

		Expected(ErrorT &&error)
			: m_error(std::move(error))
		{
		}

		T &value()
		{
			assert(m_value.has_value());
			return m_value.value();
		}

		const T &value() const
		{
			assert(m_value.has_value());
			return m_value.value();
		}

		ErrorT &error()
		{
			assert(m_error.has_value());
			return m_error.value();
		}

		const ErrorT &error() const
		{
			assert(m_error.has_value());
			return m_error.value();
		}

		bool is_error() const noexcept
		{
			return m_error.has_value();
		}

		operator bool() const noexcept
		{
			return m_error.has_value();
		}

		T &operator*()
		{
			assert(m_value.has_value());
			return m_value.value();
		}

		const T &operator*() const
		{
			assert(m_value.has_value());
			return m_value.value();
		}

		T *operator->()
		{
			assert(m_value.has_value());
			return &m_value.value();
		}

		const T *operator->() const
		{
			assert(m_value.has_value());
			return &m_value.value();
		}

	private:
		std::optional<T> m_value = {};
		std::optional<ErrorT> m_error = {};
	};

	template <typename ErrorT>
	class Expected<void, ErrorT>
	{
	public:
		Expected() = default;

		Expected(const ErrorT &error)
			: m_error(error)
		{
		}

		Expected(ErrorT &&error)
			: m_error(std::move(error))
		{
		}

		ErrorT &error()
		{
			assert(m_error.has_value());
			return m_error.value();
		}

		const ErrorT &error() const
		{
			assert(m_error.has_value());
			return m_error.value();
		}

		bool is_error() const noexcept
		{
			return m_error.has_value();
		}

		operator bool() const noexcept
		{
			return m_error.has_value();
		}

	private:
		std::optional<ErrorT> m_error = {};
	};
} // namespace HyperEngine
