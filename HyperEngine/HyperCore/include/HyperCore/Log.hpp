#pragma once

#include <HyperCore/Logger.hpp>

#include <memory>

namespace HyperCore
{
	class Log
	{
	public:
		static void Initialize();
		static void Terminate();
		
		static const Logger& GetCoreLogger();
		static const Logger& GetClientLogger();
	
	private:
		static Logger* s_pCoreLogger;
		static Logger* s_pClientLogger;
	};
}

#define HYPERENIGNE_CORE_LOG(level, ...) ::HyperCore::Log::GetCoreLogger().Log(level, __VA_ARGS__)
#define HYPERENGINE_CORE_INFO(...) HYPERENIGNE_CORE_LOG(::HyperCore::Logger::Level::Info, __VA_ARGS__)
#define HYPERENGINE_CORE_WARNING(...) HYPERENIGNE_CORE_LOG(::HyperCore::Logger::Level::Warning, __VA_ARGS__)
#define HYPERENGINE_CORE_ERROR(...) HYPERENIGNE_CORE_LOG(::HyperCore::Logger::Level::Error, __VA_ARGS__)
#define HYPERENGINE_CORE_FATAL(...) HYPERENIGNE_CORE_LOG(::HyperCore::Logger::Level::Fatal, __VA_ARGS__)
#define HYPERENGINE_CORE_DEBUG(...) HYPERENIGNE_CORE_LOG(::HyperCore::Logger::Level::Debug, __VA_ARGS__)
#define HYPERENGINE_CORE_TRACE(...) HYPERENIGNE_CORE_LOG(::HyperCore::Logger::Level::Trace, __VA_ARGS__)

#define HYPERENIGNE_CLIENT_LOG(level, ...) ::HyperCore::Log::GetCoreLogger().Log(level, __VA_ARGS__)
#define HYPERENGINE_CLIENT_INFO(...) HYPERENIGNE_CLIENT_LOG(::HyperCore::Logger::Level::Info, __VA_ARGS__)
#define HYPERENGINE_CLIENT_WARNING(...) HYPERENIGNE_CLIENT_LOG(::HyperCore::Logger::Level::Warning, __VA_ARGS__)
#define HYPERENGINE_CLIENT_ERROR(...) HYPERENIGNE_CLIENT_LOG(::HyperCore::Logger::Level::Error, __VA_ARGS__)
#define HYPERENGINE_CLIENT_FATAL(...) HYPERENIGNE_CLIENT_LOG(::HyperCore::Logger::Level::Fatal, __VA_ARGS__)
#define HYPERENGINE_CLIENT_DEBUG(...) HYPERENIGNE_CLIENT_LOG(::HyperCore::Logger::Level::Debug, __VA_ARGS__)
#define HYPERENGINE_CLIENT_TRACE(...) HYPERENIGNE_CLIENT_LOG(::HyperCore::Logger::Level::Trace, __VA_ARGS__)