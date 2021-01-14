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

#define HP_CORE_INFO(...)    ::Hyperion::Log::GetCoreLogger()->Print(Logger::Level::HP_LEVEL_INFO, __VA_ARGS__)
#define HP_CORE_WARN(...)    ::Hyperion::Log::GetCoreLogger()->Print(Logger::Level::HP_LEVEL_WARN, __VA_ARGS__)
#define HP_CORE_ERROR(...)   ::Hyperion::Log::GetCoreLogger()->Print(Logger::Level::HP_LEVEL_ERROR, __VA_ARGS__)
#define HP_CORE_FATAL(...)   ::Hyperion::Log::GetCoreLogger()->Print(Logger::Level::HP_LEVEL_FATAL, __VA_ARGS__)
#define HP_CORE_DEBUG(...)   ::Hyperion::Log::GetCoreLogger()->Print(Logger::Level::HP_LEVEL_DEBUG, __VA_ARGS__)

#define HP_CLIENT_INFO(...)  ::Hyperion::Log::GetClientLogger()->Print(Logger::Level::HP_LEVEL_INFO, __VA_ARGS__)
#define HP_CLIENT_WARN(...)  ::Hyperion::Log::GetClientLogger()->Print(Logger::Level::HP_LEVEL_WARN, __VA_ARGS__)
#define HP_CLIENT_ERROR(...) ::Hyperion::Log::GetClientLogger()->Print(Logger::Level::HP_LEVEL_ERROR, __VA_ARGS__)
#define HP_CLIENT_FATAL(...) ::Hyperion::Log::GetClientLogger()->Print(Logger::Level::HP_LEVEL_FATAL, __VA_ARGS__)
#define HP_CLIENT_DEBUG(...) ::Hyperion::Log::GetClientLogger()->Print(Logger::Level::HP_LEVEL_DEBUG, __VA_ARGS__)
