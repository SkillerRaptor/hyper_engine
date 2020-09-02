#pragma once

#include <thread>
#include <Windows.h>

#include "Logger.h"

namespace Hyperion 
{
	class Log
	{
	private:
		std::thread m_LoggerThread;
		std::queue<std::pair<Level, std::string>> m_MessageQueue;
		std::shared_ptr<Logger> m_CoreLogger;
		std::shared_ptr<Logger> m_ClientLogger;

		bool m_Running;

		static std::shared_ptr<Log> m_Instance;

	public:
		Log();
		~Log();

		static const std::shared_ptr<Log> GetInstace();

		const std::shared_ptr<Logger> GetCoreLogger() const { return m_CoreLogger; }
		const std::shared_ptr<Logger> GetClientLogger() const { return m_ClientLogger; }

		void Shutdown();

	private:
		void Init();
		void RunThread();
	};
}

#define HP_CORE_INFO(...)    ::Hyperion::Log::GetInstace()->GetCoreLogger()->PrintInfo(__VA_ARGS__)
#define HP_CORE_TRACE(...)   ::Hyperion::Log::GetInstace()->GetCoreLogger()->PrintTrace(__VA_ARGS__)
#define HP_CORE_WARN(...)    ::Hyperion::Log::GetInstace()->GetCoreLogger()->PrintWarn(__VA_ARGS__)
#define HP_CORE_ERROR(...)   ::Hyperion::Log::GetInstace()->GetCoreLogger()->PrintError(__VA_ARGS__)
#define HP_CORE_FATAL(...)   ::Hyperion::Log::GetInstace()->GetCoreLogger()->PrintFatal(__VA_ARGS__)
#define HP_CORE_DEBUG(...)   ::Hyperion::Log::GetInstace()->GetCoreLogger()->PrintDebug(__VA_ARGS__)

#define HP_CLIENT_INFO(...)  ::Hyperion::Log::GetInstace()->GetClientLogger()->PrintInfo(__VA_ARGS__)
#define HP_CLIENT_TRACE(...) ::Hyperion::Log::GetInstace()->GetClientLogger()->PrintTrace(__VA_ARGS__)
#define HP_CLIENT_WARN(...)  ::Hyperion::Log::GetInstace()->GetClientLogger()->PrintWarn(__VA_ARGS__)
#define HP_CLIENT_ERROR(...) ::Hyperion::Log::GetInstace()->GetClientLogger()->PrintError(__VA_ARGS__)
#define HP_CLIENT_FATAL(...) ::Hyperion::Log::GetInstace()->GetClientLogger()->PrintFatal(__VA_ARGS__)
#define HP_CLIENT_DEBUG(...) ::Hyperion::Log::GetInstace()->GetClientLogger()->PrintDebug(__VA_ARGS__)