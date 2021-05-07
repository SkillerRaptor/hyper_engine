#pragma once

#include <HyperCore/Logger.hpp>

#include <memory>

namespace HyperEngine
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

#define HYPERENGINE_CORE_LOG(level, ...) ::HyperEngine::Log::GetCoreLogger().Log(level, __VA_ARGS__)
#define HYPERENGINE_CORE_INFO(...) HYPERENGINE_CORE_LOG(::HyperEngine::Logger::Level::Info, __VA_ARGS__)
#define HYPERENGINE_CORE_WARNING(...) HYPERENGINE_CORE_LOG(::HyperEngine::Logger::Level::Warning, __VA_ARGS__)
#define HYPERENGINE_CORE_ERROR(...) HYPERENGINE_CORE_LOG(::HyperEngine::Logger::Level::Error, __VA_ARGS__)
#define HYPERENGINE_CORE_FATAL(...) HYPERENGINE_CORE_LOG(::HyperEngine::Logger::Level::Fatal, __VA_ARGS__)
#define HYPERENGINE_CORE_DEBUG(...) HYPERENGINE_CORE_LOG(::HyperEngine::Logger::Level::Debug, __VA_ARGS__)
#define HYPERENGINE_CORE_TRACE(...) HYPERENGINE_CORE_LOG(::HyperEngine::Logger::Level::Trace, __VA_ARGS__)

#define HYPERENGINE_CLIENT_LOG(level, ...) ::HyperEngine::Log::GetClientLogger().Log(level, __VA_ARGS__)
#define HYPERENGINE_CLIENT_INFO(...) HYPERENGINE_CLIENT_LOG(::HyperEngine::Logger::Level::Info, __VA_ARGS__)
#define HYPERENGINE_CLIENT_WARNING(...) HYPERENGINE_CLIENT_LOG(::HyperEngine::Logger::Level::Warning, __VA_ARGS__)
#define HYPERENGINE_CLIENT_ERROR(...) HYPERENGINE_CLIENT_LOG(::HyperEngine::Logger::Level::Error, __VA_ARGS__)
#define HYPERENGINE_CLIENT_FATAL(...) HYPERENGINE_CLIENT_LOG(::HyperEngine::Logger::Level::Fatal, __VA_ARGS__)
#define HYPERENGINE_CLIENT_DEBUG(...) HYPERENGINE_CLIENT_LOG(::HyperEngine::Logger::Level::Debug, __VA_ARGS__)
#define HYPERENGINE_CLIENT_TRACE(...) HYPERENGINE_CLIENT_LOG(::HyperEngine::Logger::Level::Trace, __VA_ARGS__)
