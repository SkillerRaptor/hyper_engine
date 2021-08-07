/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperCore
{
	class SourceLocation
	{
	public:
		constexpr SourceLocation() = default;
		
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
		
		static constexpr SourceLocation current(
			const char* file = __builtin_FILE(),
			const char* function = __builtin_FUNCTION(),
			uint32_t line = __builtin_LINE()) noexcept
		{
			SourceLocation location{};
			location.m_file = file;
			location.m_function = function;
			location.m_line = line;
			
			return location;
		}
	
	private:
		const char* m_file{ "unknown" };
		const char* m_function{ "unknown" };
		uint32_t m_line{ 0 };
	};
} // namespace HyperCore
