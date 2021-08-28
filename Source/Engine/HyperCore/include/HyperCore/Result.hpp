/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Errors.hpp"

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
		Result(ValueType&& t_result)
			: m_result(std::move(t_result))
		{
		}

		Result(const ValueType& t_result)
			: m_result(t_result)
		{
		}

		Result(ErrorType&& t_error)
			: m_error(std::move(t_error))
		{
		}

		Result(const ErrorType& t_error)
			: m_error(t_error)
		{
		}

		auto value() -> ValueType&
		{
			return m_result.value();
		}

		auto error() -> ErrorType&
		{
			return m_error.value();
		}

		auto is_error() const noexcept -> bool
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
		constexpr Result() = default;

		Result(ErrorType&& t_error)
			: m_error(std::move(t_error))
		{
		}

		Result(const ErrorType& t_error)
			: m_error(t_error)
		{
		}

		auto error() -> ErrorType&
		{
			return m_error.value();
		}

		auto is_error() const noexcept -> bool
		{
			return m_error.has_value();
		}

	private:
		std::optional<ErrorType> m_error{};
	};

	using InitializeResult = Result<void, ConstructError>;
	using RuntimeResult = Result<void, RuntimeError>;
} // namespace HyperCore
