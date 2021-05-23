#pragma once

#include <string>

#include <fmt/format.h>

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
		static void Info(const std::string& string, Args&&... args)
		{
			Log(Level::Info, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void Warning(const std::string& string, Args&&... args)
		{
			Log(Level::Warning, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void Error(const std::string& string, Args&&... args)
		{
			Log(Level::Error, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void Fatal(const std::string& string, Args&&... args)
		{
			Log(Level::Fatal, string, std::forward<Args&&>(args)...);
		}
		
		template <typename... Args>
		static void Debug(const std::string& string, Args&&... args)
		{
			Log(Level::Debug, string, std::forward<Args&&>(args)...);
		}
		
	private:
		template <typename... Args>
		static void Log(Level level, const std::string& string, Args&&... args)
		{
			constexpr size_t argsCount{ sizeof...(Args) };
			if constexpr (argsCount > 0)
			{
				InternalLog(level, fmt::format(string, std::forward<Args>(args)...));
			}
			else
			{
				InternalLog(level, string);
			}
		}
		
		static void InternalLog(Level level, const std::string& string);
	};
}
