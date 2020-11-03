#include "Log.hpp"

namespace Hyperion 
{
	Ref<Log> Log::m_Instance;

	Log::Log()
	{
		Init();
	}

	Log::~Log()
	{
		Shutdown();
	}

	const Ref<Log> Log::GetInstace()
	{
		if (m_Instance == nullptr)
			m_Instance = CreateRef<Log>();
		return m_Instance;
	}

	void Log::Init()
	{
		m_Running = true;

		m_MessageQueue = std::queue<std::pair<Level, std::string>>();

		m_CoreLogger = CreateRef<Logger>(&m_MessageQueue);
		m_CoreLogger->SetName("Core");
		m_CoreLogger->SetLevel(Level::HP_LEVEL_ALL);

		m_ClientLogger = CreateRef<Logger>(&m_MessageQueue);
		m_ClientLogger->SetName("Client");
		m_ClientLogger->SetLevel(Level::HP_LEVEL_ALL);

		m_LoggerThread = std::thread(&Log::RunThread, this);
	}

	void Log::Shutdown()
	{
		m_Running = false;
		m_LoggerThread.join();
	}

	void Log::RunThread()
	{
		while (m_Running)
			while (m_MessageQueue.size() > 0)
			{
				std::pair<Level, std::string> message = m_MessageQueue.front();
				switch (message.first)
				{
				case Level::HP_LEVEL_INFO: default:
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					break;
				case Level::HP_LEVEL_TRACE:
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					break;
				case Level::HP_LEVEL_WARN:
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
					break;
				case Level::HP_LEVEL_ERROR:
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
					break;
				case Level::HP_LEVEL_FATAL:
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
					break;
				case Level::HP_LEVEL_DEBUG:
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					break;
				}

				std::time_t time = std::time(0);
				struct tm localTime;
				localtime_s(&localTime, &time);

				std::cout << "[" << (localTime.tm_hour < 10 ? "0" : "") << localTime.tm_hour << ":" << (localTime.tm_min < 10 ? "0" : "") << localTime.tm_min << ":" << (localTime.tm_sec < 10 ? "0" : "") << localTime.tm_sec << "] " << message.second.c_str();
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				m_MessageQueue.pop();
			}
	}
}
