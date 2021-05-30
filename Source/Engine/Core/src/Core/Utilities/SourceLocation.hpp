#pragma once

#include <Core/Logger.hpp>

namespace Core
{
	class SourceLocation
	{
	public:
		constexpr SourceLocation() = default;
		~SourceLocation() = default;
		
		static constexpr SourceLocation current(
			const char* file = __builtin_FILE(),
			const char* function = __builtin_FUNCTION(),
			uint32_t line = __builtin_LINE())
		{
			SourceLocation location{};
			location.m_file = file;
			location.m_function = function;
			location.m_line = line;
			
			return location;
		}
		
		constexpr const char* file_name() const
		{
			return m_file;
		}
		
		constexpr const char* function_name() const
		{
			return m_function;
		}
		
		constexpr uint32_t line() const
		{
			return m_line;
		}
	
	private:
		const char* m_file{ "unknown" };
		const char* m_function{ "unknown" };
		uint32_t m_line{ 0 };
	};
}
