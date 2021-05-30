#pragma once

#include "Formatter.hpp"

#include <string>

namespace Core
{
	class Logger
	{
	private:
		enum class Level
		{
			Info,
			Warning,
			Error,
			Fatal,
			Debug
		};
	
	public:
		template <typename... Args>
		static void log(Logger::Level log_level, const std::string& string, Args&& ... args)
		{
			internal_log(log_level, Formatter::format(string, std::forward<Args&&>(args)...));
		}
		
		template <typename... Args>
		static void info(const std::string& string, Args&& ... args)
		{
			log(Level::Info, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void warning(const std::string& string, Args&& ... args)
		{
			log(Level::Warning, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void error(const std::string& string, Args&& ... args)
		{
			log(Level::Error, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void fatal(const std::string& string, Args&& ... args)
		{
			log(Level::Fatal, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void debug(const std::string& string, Args&& ... args)
		{
			log(Level::Debug, string, std::forward<Args&&>(args)...);
		}
	
	private:
		static void internal_log(Level log_level, const std::string& string);
	};
}
