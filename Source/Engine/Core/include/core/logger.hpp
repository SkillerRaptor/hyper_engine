#pragma once

#include <string>

#include <fmt/format.h>

namespace core
{
	class logger
	{
	private:
		enum class level
		{
			info,
			warning,
			error,
			fatal,
			debug
		};
	
	public:
		template <typename... Args>
		static void info(const std::string& string, Args&& ... args)
		{
			format(level::info, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void warning(const std::string& string, Args&& ... args)
		{
			format(level::warning, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void error(const std::string& string, Args&& ... args)
		{
			format(level::error, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void fatal(const std::string& string, Args&& ... args)
		{
			format(level::fatal, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void debug(const std::string& string, Args&& ... args)
		{
			format(level::debug, string, std::forward<Args&&>(args)...);
		}
	
	private:
		template <typename... Args>
		static void format(level log_level, const std::string& string, Args&& ... args)
		{
			constexpr size_t args_count{ sizeof...(Args) };
			if constexpr (args_count > 0)
			{
				internal_log(log_level, fmt::format(string, std::forward<Args>(args)...));
				return;
			}
			
			internal_log(log_level, string);
		}
		
		static void internal_log(level log_level, const std::string& string);
	};
}
