/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Error.hpp"

#include <cassert>
#include <iostream>
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

		Expected(Error &&error)
			: m_error(std::move(error))
		{
		}

		Expected(const Expected &other) = delete;
		Expected(Expected &&other) noexcept
		{
			m_checked = true;
			m_value = std::move(other.m_value);
			m_error = std::move(other.m_error);

			other.m_checked = false;
			other.m_value = std::nullopt;
			other.m_error = std::nullopt;
		}

		Expected &operator=(const Expected &other) = delete;
		Expected &operator=(Expected &&other) noexcept
		{
			assert_is_checked();

			m_checked = false;
			m_value = std::move(other.m_value);
			m_error = std::move(other.m_error);

			other.m_checked = true;
			other.m_value = std::nullopt;
			other.m_error = std::nullopt;

			return *this;
		}

		Expected(ErrorSuccess error_success) = delete;

		~Expected()
		{
			assert_is_checked();
		}

		T &value()
		{
			assert_is_checked();
			return m_value.value();
		}

		const T &value() const
		{
			assert_is_checked();
			return m_value.value();
		}

		T *operator->()
		{
			assert_is_checked();
			return std::addressof(m_value.value());
		}

		const T *operator->() const
		{
			assert_is_checked();
			return std::addressof(m_value.value());
		}

		T &operator*()
		{
			assert_is_checked();
			return m_value.value();
		}

		const T &operator*() const
		{
			assert_is_checked();
			return m_value.value();
		}

		bool is_error()
		{
			m_checked = true;
			return m_error.has_value();
		}

	private:
		void assert_is_checked()
		{
			if (m_checked)
			{
				return;
			}

			std::abort();
		}

	private:
		bool m_checked = false;
		std::optional<T> m_value = {};
		std::optional<Error> m_error = {};
	};
} // namespace HyperEngine
