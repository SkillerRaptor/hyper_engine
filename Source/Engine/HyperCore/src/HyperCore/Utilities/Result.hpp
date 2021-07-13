/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <utility>

namespace HyperCore
{
	template <typename ValueT, typename ErrorT>
	class CResult
	{
	public:
		using ValueType = ValueT;
		using ErrorType = ErrorT;

	public:
		CResult(ValueType&& result) noexcept
			: m_result(std::move(result))
		{
		}
		
		CResult(const ValueType& result)
			: m_result(result)
		{
		}
		
		CResult(ErrorType&& error) noexcept
			: m_error(std::move(error))
		{
		}
		
		CResult(const ErrorType& error)
			: m_error(error)
		{
		}

		ValueType& value()
		{
			return m_result.value();
		}

		ErrorType& error()
		{
			return m_error.value();
		}

		bool is_error() const
		{
			return m_error.has_value();
		}

	private:
		std::optional<ValueType> m_result{};
		std::optional<ErrorType> m_error{};
	};

	template <typename ErrorT>
	class CResult<void, ErrorT>
	{
	public:
		using ValueType = void;
		using ErrorType = ErrorT;

	public:
		CResult(ErrorType&& error) noexcept
			: m_error(std::move(error))
		{
		}
		
		CResult(const ErrorType& error)
			: m_error(error)
		{
		}

		ErrorType& error()
		{
			return m_error.value();
		}

		bool is_error() const
		{
			return m_error.has_value();
		}

	private:
		std::optional<ErrorType> m_error{};
	};
} // namespace HyperCore
