/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Concepts.hpp"
#include "HyperEngine/Support/Error.hpp"

#include <cassert>
#include <optional>

namespace HyperEngine
{
	template <typename T>
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

		Expected(const Error &error)
			: m_error(error)
		{
		}

		Expected(Error &&error)
			: m_error(std::move(error))
		{
		}

		bool is_error() const noexcept
		{
			return m_error.has_value();
		}

		T &value() &
		{
			assert(m_value.has_value() && "The value can't be null");
			return m_value.value();
		}

		const T &value() const &
		{
			assert(m_value.has_value() && "The value can't be null");
			return m_value.value();
		}

		T &&value() &&
		{
			assert(m_value.has_value() && "The value can't be null");
			return std::move(m_value.value());
		}

		const T &&value() const &&
		{
			assert(m_value.has_value() && "The value can't be null");
			return std::move(m_value.value());
		}

		Error &error()
		{
			assert(m_error.has_value() && "The error can't be null");
			return m_error.value();
		}

		const Error &error() const
		{
			assert(m_error.has_value() && "The error can't be null");
			return m_error.value();
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
		std::optional<T> m_value = std::nullopt;
		std::optional<Error> m_error = std::nullopt;
	};

	template <>
	class Expected<void>
	{
	public:
		Expected() = default;

		Expected(const Error &error)
			: m_error(error)
		{
		}

		Expected(Error &&error)
			: m_error(std::move(error))
		{
		}

		bool is_error() const noexcept
		{
			return m_error.has_value();
		}

		Error &error()
		{
			assert(m_error.has_value() && "The error can't be null");
			return m_error.value();
		}

		const Error &error() const
		{
			assert(m_error.has_value() && "The error can't be null");
			return m_error.value();
		}

	private:
		std::optional<Error> m_error = std::nullopt;
	};
} // namespace HyperEngine
