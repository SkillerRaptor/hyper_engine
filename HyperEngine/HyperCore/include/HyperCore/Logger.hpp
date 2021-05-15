#pragma once

#include <fmt/format.h>

#include <string>
#include <iostream>
#include <utility>

namespace HyperCore
{
	class Logger
	{
	public:
		enum class Level
		{
			Info,
			Warning,
			Error,
			Fatal,
			Debug,
			Trace
		};
	
	public:
		template <typename... T>
		static constexpr void Info(const std::string& format, T&&... args)
		{
			Logger::Log(Logger::Level::Info, format, std::forward<T&&>(args)...);
		}
		
		template <typename... T>
		static constexpr void Warning(const std::string& format, T&&... args)
		{
			Logger::Log(Logger::Level::Warning, format, std::forward<T&&>(args)...);
		}
		
		template <typename... T>
		static constexpr void Error(const std::string& format, T&&... args)
		{
			Logger::Log(Logger::Level::Error, format, std::forward<T&&>(args)...);
		}
		
		template <typename... T>
		static constexpr void Fatal(const std::string& format, T&&... args)
		{
			Logger::Log(Logger::Level::Fatal, format, std::forward<T&&>(args)...);
		}
		
		template <typename... T>
		static constexpr void Debug(const std::string& format, T&&... args)
		{
			Logger::Log(Logger::Level::Debug, format, std::forward<T&&>(args)...);
		}
		
		template <typename... Args>
		static constexpr void Log(Level level, std::string_view format, Args&& ... args)
		{
			if (s_level != Logger::Level::Trace && s_level != level)
			{
				return;
			}
			
			constexpr size_t argsCount{ sizeof...(Args) };
			if constexpr (argsCount > 0)
			{
				LogInternal(level, fmt::format(format, std::forward<Args>(args)...));
			}
			else
			{
				LogInternal(level, format);
			}
		}
		
		static void SetLevel(Level level);
		static Level GetLevel();
	
	private:
		static void LogInternal(Level level, std::string_view format);
	
	private:
		static Level s_level;
	};
}
