/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>

namespace HyperEngine
{
	class CSourceLocation
	{
	public:
		[[nodiscard]] constexpr auto file_name() const noexcept -> const char*
		{
			return m_file;
		}

		[[nodiscard]] constexpr auto function_name() const noexcept -> const char*
		{
			return m_function;
		}

		[[nodiscard]] constexpr auto line() const noexcept -> size_t
		{
			return m_line;
		}

		[[nodiscard]] static constexpr auto current(
			const char* file = __builtin_FILE(),
			const char* function = __builtin_FUNCTION(),
			const size_t line = __builtin_LINE()) noexcept -> CSourceLocation
		{
			CSourceLocation location = {};
			location.m_file = file;
			location.m_function = function;
			location.m_line = line;

			return location;
		}

	private:
		constexpr CSourceLocation() = default;

	private:
		const char* m_file = { "unknown" };
		const char* m_function = { "unknown" };
		size_t m_line = { 0 };
	};
} // namespace HyperEngine