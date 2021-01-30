#pragma once

#include <fmt/core.h>

#include <string>
#include <thread>
#include <mutex>
#include <utility>
#include <queue>

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

		struct Message
		{
			size_t Id;
			std::string Content;
			Level ContentLevel;
			std::string LoggerName;
		};

	private:
		std::string m_Name;
		Level m_Level;

		static bool s_Running;
		static size_t s_CurrentMessage;
		static std::thread s_LoggerThread;
		static std::queue<Message> s_MessageQueue;
		static std::mutex s_MessageQueueMutex;

	public:
		Logger();
		Logger(const std::string& name, Level level);

		static void Run();
		static void Shutdown();

		template<typename... Args>
		size_t Print(Level level, const std::string& format, Args&&... args)
		{
			if (m_Level != Level::HP_LEVEL_TRACE)
				if (m_Level != level)
					return -1;

			std::string content = format;

			size_t argsCount = sizeof...(Args);
			if (argsCount != 0)
				content = fmt::format(content, std::forward<Args>(args)...);

			size_t messageId = s_MessageQueue.size() + 1;

			Message message{};
			message.Id = messageId;
			message.Content = content;
			message.ContentLevel = level;
			message.LoggerName = m_Name;
			const std::lock_guard<std::mutex> lock(s_MessageQueueMutex);
			s_MessageQueue.push(message);

			return messageId;
		}

		static void WaitForMessage(size_t messageId);

		void SetName(const std::string& name);
		const std::string& GetName() const;

		void SetLevel(Level level);
		Level GetLevel() const;

	private:
		static const char* GetLevelName(Level level);
	};
}
