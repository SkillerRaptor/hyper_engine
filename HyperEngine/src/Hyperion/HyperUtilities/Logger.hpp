#pragma once

#include <fmt/core.h>

#include <sstream>
#include <string>

namespace Hyperion 
{
	class Logger
	{
	public:
		enum class Level
		{
			HP_LEVEL_INFO,
			HP_LEVEL_WARN,
			HP_LEVEL_ERROR,
			HP_LEVEL_FATAL,
			HP_LEVEL_DEBUG,
			HP_LEVEL_TRACE
		};

	private:
		std::string m_Name;
		Level m_Level;

	public:
		Logger();

		void Print(Level level, const std::string& format);

		template<typename... Args>
		void Print(Level level, const std::string& format, Args&&... args)
		{
			std::string string = fmt::format(format, std::forward<Args>(args)...);

			PrintInternal(level, string);
		}

		void SetName(const std::string& name);
		const std::string& GetName() const;

		void SetLevel(Level level);
		const Level GetLevel() const;

	private:
		void PrintInternal(Level level, const std::string& message);

		std::string ConvertLevelToString(Level level) const;
	};
}
