#pragma once

#include <sstream>
#include <string>

namespace Hyperion 
{
	enum class Level
	{
		HP_LEVEL_DEBUG,
		HP_LEVEL_ERROR,
		HP_LEVEL_FATAL,
		HP_LEVEL_INFO,
		HP_LEVEL_TRACE,
		HP_LEVEL_WARN
	};

	class Logger
	{
	private:
		std::string m_Name;
		Level m_Level;

	public:
		Logger();
		~Logger();

		void PrintInfo(const char* format);

		template<typename T, typename... Targs>
		void PrintInfo(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_LEVEL_INFO, format, value, std::forward<Targs>(Fargs)...);
		}

		void PrintWarn(const char* format);

		template<typename T, typename... Targs>
		void PrintWarn(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_LEVEL_WARN, format, value, std::forward<Targs>(Fargs)...);
		}

		void PrintError(const char* format);

		template<typename T, typename... Targs>
		void PrintError(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_LEVEL_ERROR, format, value, std::forward<Targs>(Fargs)...);
		}

		void PrintFatal(const char* format);

		template<typename T, typename... Targs>
		void PrintFatal(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_LEVEL_FATAL, format, value, std::forward<Targs>(Fargs)...);
		}

		void PrintDebug(const char* format);

		template<typename T, typename... Targs>
		void PrintDebug(const char* format, T&& value, Targs&&... Fargs)
		{
			Print(Level::HP_LEVEL_DEBUG, format, value, std::forward<Targs>(Fargs)...);
		}

		template<typename T, typename... Targs>
		void Print(Level level, const char* format, T&& value, Targs&&... Fargs)
		{
			std::stringstream ss;

			FormatString(ss, format, value, std::forward<Targs>(Fargs)...);

			Print(level, ss.str());
		}

		void FormatString(std::stringstream& ss, const char* format);

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

		const void SetName(std::string name);
		const std::string GetName() const;

		const void SetLevel(Level level);
		const Level GetLevel() const;

	private:
		void Print(Level level, const char* format);
		void Print(Level level, const std::string& message);

		std::string ConvertLevelToString(Level level) const;
	};
}
