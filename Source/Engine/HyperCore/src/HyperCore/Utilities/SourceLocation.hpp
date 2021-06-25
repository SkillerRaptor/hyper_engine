/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Logger.hpp>

namespace HyperCore
{
	class CSourceLocation
	{
	public:
		constexpr CSourceLocation() = default;

		static constexpr CSourceLocation current(
			const char* file = __builtin_FILE(),
			const char* function = __builtin_FUNCTION(),
			uint32_t line = __builtin_LINE())
		{
			CSourceLocation location{};
			location.m_file = file;
			location.m_function = function;
			location.m_line = line;

			return location;
		}

		constexpr const char* file_name() const noexcept
		{
			return m_file;
		}

		constexpr const char* function_name() const noexcept
		{
			return m_function;
		}

		constexpr uint32_t line() const noexcept
		{
			return m_line;
		}

	private:
		const char* m_file{ "unknown" };
		const char* m_function{ "unknown" };
		uint32_t m_line{ 0 };
	};
} // namespace HyperCore
