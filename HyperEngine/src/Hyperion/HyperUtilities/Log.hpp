#pragma once

#include "HyperCore/Core.hpp"
#include "HyperUtilities/Logger.hpp"

namespace Hyperion 
{
	class Log
	{
	private:
		static Ref<Logger> m_CoreLogger;
		static Ref<Logger> m_ClientLogger;

	public:
		static void Init();

		static const Ref<Logger> GetCoreLogger() { return m_CoreLogger; }
		static const Ref<Logger> GetClientLogger() { return m_ClientLogger; }
	};
}

#define HP_CORE_INFO(...)    ::Hyperion::Log::GetCoreLogger()->PrintInfo(__VA_ARGS__)
#define HP_CORE_TRACE(...)   ::Hyperion::Log::GetCoreLogger()->PrintTrace(__VA_ARGS__)
#define HP_CORE_WARN(...)    ::Hyperion::Log::GetCoreLogger()->PrintWarn(__VA_ARGS__)
#define HP_CORE_ERROR(...)   ::Hyperion::Log::GetCoreLogger()->PrintError(__VA_ARGS__)
#define HP_CORE_FATAL(...)   ::Hyperion::Log::GetCoreLogger()->PrintFatal(__VA_ARGS__)
#define HP_CORE_DEBUG(...)   ::Hyperion::Log::GetCoreLogger()->PrintDebug(__VA_ARGS__)

#define HP_CLIENT_INFO(...)  ::Hyperion::Log::GetClientLogger()->PrintInfo(__VA_ARGS__)
#define HP_CLIENT_TRACE(...) ::Hyperion::Log::GetClientLogger()->PrintTrace(__VA_ARGS__)
#define HP_CLIENT_WARN(...)  ::Hyperion::Log::GetClientLogger()->PrintWarn(__VA_ARGS__)
#define HP_CLIENT_ERROR(...) ::Hyperion::Log::GetClientLogger()->PrintError(__VA_ARGS__)
#define HP_CLIENT_FATAL(...) ::Hyperion::Log::GetClientLogger()->PrintFatal(__VA_ARGS__)
#define HP_CLIENT_DEBUG(...) ::Hyperion::Log::GetClientLogger()->PrintDebug(__VA_ARGS__)
