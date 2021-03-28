#pragma once

#include <fmt/format.h>

#include <type_traits>
#include <string_view>

namespace HyperEngine
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
		Logger(std::string  name, Level level)
			: m_name{ std::move( name ) }
			, m_level{ level }
		{
		}
		
		template <typename... Args>
		void LogInfo(const std::string_view format, Args... args)
		{
			if (m_level != Level::Trace && m_level != Level::Info)
			{
				return;
			}
			
			constexpr size_t argsCount{ sizeof...(Args) };
			if constexpr (argsCount > 0)
			{
				LogInternal(Level::Info, fmt::format(format, std::forward<Args>(args)...));
			}
			else
			{
				LogInternal(Level::Info, format);
			}
		}
		
		template <typename... Args>
		void LogWarning(const std::string_view format, Args&& ... args)
		{
			if (m_level != Level::Trace && m_level != Level::Warning)
			{
				return;
			}
			
			constexpr size_t argsCount{ sizeof...(Args) };
			if constexpr (argsCount > 0)
			{
				LogInternal(Level::Warning, fmt::format(format, std::forward<Args>(args)...));
			}
			else
			{
				LogInternal(Level::Warning, format);
			}
		}
		
		template <typename... Args>
		void LogError(const std::string_view format, Args&& ... args)
		{
			if (m_level != Level::Trace && m_level != Level::Error)
			{
				return;
			}
			
			constexpr size_t argsCount{ sizeof...(Args) };
			if constexpr (argsCount > 0)
			{
				LogInternal(Level::Error, fmt::format(format, std::forward<Args>(args)...));
			}
			else
			{
				LogInternal(Level::Error, format);
			}
		}
		
		template <typename... Args>
		void LogFatal(const std::string_view format, Args&& ... args)
		{
			if (m_level != Level::Trace && m_level != Level::Fatal)
			{
				return;
			}
			
			constexpr size_t argsCount{ sizeof...(Args) };
			if constexpr (argsCount > 0)
			{
				LogInternal(Level::Fatal, fmt::format(format, std::forward<Args>(args)...));
			}
			else
			{
				LogInternal(Level::Fatal, format);
			}
		}
		
		template <typename... Args>
		void LogDebug(const std::string_view format, Args&& ... args)
		{
			if (m_level != Level::Trace && m_level != Level::Debug)
			{
				return;
			}
			
			constexpr size_t argsCount{ sizeof...(Args) };
			if constexpr (argsCount > 0)
			{
				LogInternal(Level::Debug, fmt::format(format, std::forward<Args>(args)...));
			}
			else
			{
				LogInternal(Level::Debug, format);
			}
		}
		
		template <typename... Args>
		void LogTrace(const std::string_view format, Args&& ... args)
		{
			if (m_level != Level::Trace)
			{
				return;
			}
			
			constexpr size_t argsCount{ sizeof...(Args) };
			if constexpr (argsCount > 0)
			{
				LogInternal(Level::Trace, fmt::format(format, std::forward<Args>(args)...));
			}
			else
			{
				LogInternal(Level::Trace, format);
			}
		}
		
		inline void SetName(const std::string_view name) { m_name = name; }
		inline std::string_view GetName() const { return m_name; }
		
		inline void SetLevel(Level level) { m_level = level; }
		inline Level GetLevel() const { return m_level; }
	
	private:
		void LogInternal(Level level, std::string_view format);
	
	private:
		std::string m_name;
		Level m_level;
	};
}

