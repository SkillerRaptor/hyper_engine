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
		Logger(std::string name, Level level)
			: m_name{ std::move(name) }
			, m_level{ level }
		{
		}
		
		template <typename... Args>
		constexpr void Log(Level level, std::string_view format, Args&& ... args) const
		{
			if (m_level != Level::Trace && m_level != level)
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
		
		void SetName(const std::string& name);
		std::string GetName() const;
		
		void SetLevel(Level level);
		Level GetLevel() const;
	
	private:
		void LogInternal(Level level, std::string_view format) const;
	
	private:
		std::string m_name;
		Level m_level;
	};
}
