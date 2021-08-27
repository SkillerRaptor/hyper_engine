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
		Result(ValueType&& result) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
			: m_result(std::move(result))
		{
		}

		Result(const ValueType& result) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
			: m_result(result)
		{
		}

		Result(ErrorType&& error) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
			: m_error(std::move(error))
		{
		}

		Result(const ErrorType& error) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
			: m_error(error)
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
		Result() = default;

		Result(ErrorType&& error) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
			: m_error(std::move(error))
		{
		}

		Result(const ErrorType& error) // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
			: m_error(error)
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
	
	using InitializeResult = Result<void, Errors::ConstructError>;
	using RuntimeResult = Result<void, Errors::RuntimeError>;
	
} // namespace HyperCore
