#pragma once

#include <HyperCore/Logger.hpp>

#include <memory>

namespace HyperCore
{
	class Log
	{
	public:
		static void Initialize();
		
		static const std::shared_ptr<Logger>& GetCoreLogger();
		static const std::shared_ptr<Logger>& GetClientLogger();
	
	private:
		static std::shared_ptr<Logger> s_pCoreLogger;
		static std::shared_ptr<Logger> s_pClientLogger;
	};
}

#define HYPERENGINE_CORE_LOG(level, ...) ::HyperCore::Log::GetCoreLogger()->Log(level, __VA_ARGS__)
#define HYPERENGINE_CORE_INFO(...) HYPERENGINE_CORE_LOG(::HyperCore::Logger::Level::Info, __VA_ARGS__)
#define HYPERENGINE_CORE_WARNING(...) HYPERENGINE_CORE_LOG(::HyperCore::Logger::Level::Warning, __VA_ARGS__)
#define HYPERENGINE_CORE_ERROR(...) HYPERENGINE_CORE_LOG(::HyperCore::Logger::Level::Error, __VA_ARGS__)
#define HYPERENGINE_CORE_FATAL(...) HYPERENGINE_CORE_LOG(::HyperCore::Logger::Level::Fatal, __VA_ARGS__)
#define HYPERENGINE_CORE_DEBUG(...) HYPERENGINE_CORE_LOG(::HyperCore::Logger::Level::Debug, __VA_ARGS__)
#define HYPERENGINE_CORE_TRACE(...) HYPERENGINE_CORE_LOG(::HyperCore::Logger::Level::Trace, __VA_ARGS__)

#define HYPERENGINE_CLIENT_LOG(level, ...) ::HyperCore::Log::GetClientLogger()->Log(level, __VA_ARGS__)
#define HYPERENGINE_CLIENT_INFO(...) HYPERENGINE_CLIENT_LOG(::HyperCore::Logger::Level::Info, __VA_ARGS__)
#define HYPERENGINE_CLIENT_WARNING(...) HYPERENGINE_CLIENT_LOG(::HyperCore::Logger::Level::Warning, __VA_ARGS__)
#define HYPERENGINE_CLIENT_ERROR(...) HYPERENGINE_LOG(::HyperCore::Logger::Level::Error, __VA_ARGS__)
#define HYPERENGINE_CLIENT_FATAL(...) HYPERENGINE_CLIENT_LOG(::HyperCore::Logger::Level::Fatal, __VA_ARGS__)
#define HYPERENGINE_CLIENT_DEBUG(...) HYPERENGINE_CLIENT_LOG(::HyperCore::Logger::Level::Debug, __VA_ARGS__)
#define HYPERENGINE_CLIENT_TRACE(...) HYPERENGINE_CLIENT_LOG(::HyperCore::Logger::Level::Trace, __VA_ARGS__)
