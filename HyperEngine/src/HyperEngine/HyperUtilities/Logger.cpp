#include "HyperUtilities/Logger.hpp"

#include <fmt/color.h>
#include <fmt/chrono.h>

namespace HyperUtilities
{
	std::atomic<bool> Logger::s_Running = true;
	std::atomic<size_t> Logger::s_CurrentMessage = 0;
	std::thread Logger::s_LoggerThread = std::thread(&Logger::Run);
	std::queue<Logger::Message> Logger::s_MessageQueue = {};
	std::mutex Logger::s_MessageQueueMutex;

	Logger::Logger()
		: m_Name{ "Undefined" }, m_Level{ Level::HP_LEVEL_TRACE }
	{
	}

	Logger::Logger(const std::string& name, Level level)
		: m_Name{ name }, m_Level{ level }
	{
	}

	void Logger::Run()
	{
		while (s_Running)
		{
			while (!s_MessageQueue.empty())
			{
				const std::lock_guard<std::mutex> lock{ s_MessageQueueMutex };
				Message message = s_MessageQueue.front();
				s_MessageQueue.pop();

				Level level = message.ContentLevel;

				fmt::color levelColor;
				switch (level)
				{
				case Level::HP_LEVEL_INFO:
					levelColor = fmt::color::white;
					break;
				case Level::HP_LEVEL_WARN:
					levelColor = fmt::color::gold;
					break;
				case Level::HP_LEVEL_ERROR:
					levelColor = fmt::color::red;
					break;
				case Level::HP_LEVEL_FATAL:
					levelColor = fmt::color::crimson;
					break;
				case Level::HP_LEVEL_DEBUG:
					levelColor = fmt::color::gray;
					break;
				default:
					levelColor = fmt::color::white;
					break;
				}

				fmt::print(fg(levelColor), "[{:%H:%M:%S}] {} {}: {}\n", fmt::localtime(std::time(nullptr)), message.LoggerName, GetLevelName(level), message.Content);
				s_CurrentMessage = message.Id;
			}
		}
	}

	void Logger::Shutdown()
	{
		while (!s_MessageQueue.empty());

		s_Running = false;
		s_LoggerThread.join();
	}

	const char* Logger::GetLevelName(Level level)
	{
		switch (level)
		{
		case Level::HP_LEVEL_INFO:
			return "Info";
		case Level::HP_LEVEL_WARN:
			return "Warn";
		case Level::HP_LEVEL_ERROR:
			return "Error";
		case Level::HP_LEVEL_FATAL:
			return "Fatal";
		case Level::HP_LEVEL_DEBUG:
			return "Debug";
		default:
			return "Undefined";
		}
	}

	void Logger::WaitForMessage(size_t messageId)
	{
		while (messageId != s_CurrentMessage);
	}
}
