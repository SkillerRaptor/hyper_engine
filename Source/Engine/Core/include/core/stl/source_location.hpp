#pragma once

#include <core/logger.hpp>

namespace core
{
	class source_location
	{
	public:
		constexpr source_location() noexcept
			: m_file{ "unknown" }
			, m_function{ "unknown" }
			, m_line{ 0 }
		{
		}
		
		static constexpr source_location current(
			const char* file = __builtin_FILE(),
			const char* function = __builtin_FUNCTION(),
			uint32_t line = __builtin_LINE())
		{
			source_location location{};
			location.m_function = function;
			location.m_line = line;
			
			const char* file_name = file;
			while (*file)
			{
				const char next_char = *(file++);
				if (next_char == '/' || next_char == '\\')
				{
					file_name = file;
				}
			}
			location.m_file = file_name;
			
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
		const char* m_file;
		const char* m_function;
		uint32_t m_line;
	};
}
