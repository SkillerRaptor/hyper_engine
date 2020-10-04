#pragma once

#include <ctime>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <queue>

namespace Hyperion 
{
	enum class Level
	{
		HP_ALL,
		HP_INFO,
		HP_WARN,
		HP_TRACE,
		HP_ERROR,
		HP_FATAL,
		HP_DEBUG
	};

	class Logger
	{
	private:
		std::string m_Name;
		Level m_Level;

		std::queue<std::pair<Level, std::string>>* m_MessageQueue;

	public:
		Logger(std::queue<std::pair<Level, std::string>>*);
		~Logger();

		void PrintInfo(const char* format);
		void PrintTrace(const char* format);
		void PrintWarn(const char* format);
		void PrintError(const char* format);
		void PrintFatal(const char* format);
		void PrintDebug(const char* format);

		template<typename T, typename... Targs>
		void PrintInfo(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_INFO, format, value, std::forward<Targs>(Fargs)...);
		}

		template<typename T, typename... Targs>
		void PrintTrace(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_TRACE, format, value, std::forward<Targs>(Fargs)...);
		}

		template<typename T, typename... Targs>
		void PrintWarn(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_WARN, format, value, std::forward<Targs>(Fargs)...);
		}

		template<typename T, typename... Targs>
		void PrintError(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_ERROR, format, value, std::forward<Targs>(Fargs)...);
		}

		template<typename T, typename... Targs>
		void PrintFatal(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_FATAL, format, value, std::forward<Targs>(Fargs)...);
		}

		template<typename T, typename... Targs>
		void PrintDebug(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_DEBUG, format, value, std::forward<Targs>(Fargs)...);
		}

		template<typename T, typename... Targs>
		void Print(Level level, const char* format, T&& value, Targs&&... Fargs)
		{
			std::stringstream ss;

			ss << m_Name << " " << ConvertLevelToString(level) << ": ";
			FormatString(ss, format, value, std::forward<Targs>(Fargs)...);

			ss << std::endl;

			m_MessageQueue->push(std::make_pair(level, ss.str()));
		}

		void FormatString(std::stringstream& ss, const char* format)
		{
			ss << format;
		}

		template<typename T, typename... Targs>
		void FormatString(std::stringstream& ss, const char* format, T&& value, Targs&&... Fargs)
		{
			for (; *format != '\0'; format++) {
				if (*format == '%') {
					ss << value;
					FormatString(ss, format + 1, Fargs...);
					return;
				}
				ss << *format;
			}
		}

		const void SetName(std::string name) { m_Name = name; }
		const void SetLevel(Level level) { m_Level = level; }

		const std::string GetName() const { return m_Name; }
		const Level GetLevel() const { return m_Level; }

	private:
		void Print(Level level, const char* format);
		std::string ConvertLevelToString(Level level);
		char* ConvertNumber(unsigned int number, int base);
	};
}