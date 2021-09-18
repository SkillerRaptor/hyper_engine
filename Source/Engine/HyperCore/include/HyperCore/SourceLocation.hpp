/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>

namespace HyperEngine
{
	class SourceLocation
	{
	public:
		constexpr auto file_name() const noexcept -> const char*
		{
			return m_file;
		}

		constexpr auto function_name() const noexcept -> const char*
		{
			return m_function;
		}

		constexpr auto line() const noexcept -> size_t
		{
			return m_line;
		}

		static constexpr auto current(
			const char* file = __builtin_FILE(),
			const char* function = __builtin_FUNCTION(),
			const size_t line = __builtin_LINE()) noexcept -> SourceLocation
		{
			SourceLocation location{};
			location.m_file = file;
			location.m_function = function;
			location.m_line = line;

			return location;
		}

	private:
		constexpr SourceLocation() = default;

	private:
		const char* m_file{ "unknown" };
		const char* m_function{ "unknown" };
		size_t m_line{ 0 };
	};
} // namespace HyperEngine
