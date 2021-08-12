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
	class Result
	{
	public:
		using ValueType = ValueT;
		using ErrorType = ErrorT;

	public:
		Result(ValueType&& result)
			: m_result(std::move(result))
		{
		}

		Result(const ValueType& result)
			: m_result(result)
		{
		}

		Result(ErrorType&& error)
			: m_error(std::move(error))
		{
		}

		Result(const ErrorType& error)
			: m_error(error)
		{
		}

		Result(Result&& other) = default;
		Result(const Result& other) = default;
		~Result() = default;

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
	class Result<void, ErrorT>
	{
	public:
		using ValueType = void;
		using ErrorType = ErrorT;

	public:
		Result(ErrorType&& error)
			: m_error(std::move(error))
		{
		}

		Result(const ErrorType& error)
			: m_error(error)
		{
		}

		Result() = default;
		Result(Result&& other) = default;
		Result(const Result& other) = default;
		~Result() = default;

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
