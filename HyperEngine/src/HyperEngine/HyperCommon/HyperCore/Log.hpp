#pragma once

#include "Logger.hpp"

#include <memory>

namespace HyperEngine
{
	class Log
	{
	public:
		static void Init();

		static inline const std::shared_ptr<Logger>& GetCoreLogger() { return s_pCoreLogger; }
		static inline const std::shared_ptr<Logger>& GetClientLogger() { return s_pClientLogger; }
		
	private:
		static std::shared_ptr<Logger> s_pCoreLogger;
		static std::shared_ptr<Logger> s_pClientLogger;
	};
}

#define HYPERENGINE_CORE_INFO(...) ::HyperEngine::Log::GetCoreLogger()->LogInfo(__VA_ARGS__)
#define HYPERENGINE_CORE_WARNING(...) ::HyperEngine::Log::GetCoreLogger()->LogWarning(__VA_ARGS__)
#define HYPERENGINE_CORE_ERROR(...) ::HyperEngine::Log::GetCoreLogger()->LogError(__VA_ARGS__)
#define HYPERENGINE_CORE_FATAL(...) ::HyperEngine::Log::GetCoreLogger()->LogFatal(__VA_ARGS__)
#define HYPERENGINE_CORE_DEBUG(...) ::HyperEngine::Log::GetCoreLogger()->LogDebug(__VA_ARGS__)
#define HYPERENGINE_CORE_TRACE(...) ::HyperEngine::Log::GetCoreLogger()->LogTrace(__VA_ARGS__)

#define HYPERENGINE_CLIENT_INFO(...) ::HyperEngine::Log::GetClientLogger()->LogInfo(__VA_ARGS__)
#define HYPERENGINE_CLIENT_WARNING(...) ::HyperEngine::Log::GetClientLogger()->LogWarning(__VA_ARGS__)
#define HYPERENGINE_CLIENT_ERROR(...) ::HyperEngine::Log::GetClientLogger()->LogError(__VA_ARGS__)
#define HYPERENGINE_CLIENT_FATAL(...) ::HyperEngine::Log::GetClientLogger()->LogFatal(__VA_ARGS__)
#define HYPERENGINE_CLIENT_DEBUG(...) ::HyperEngine::Log::GetClientLogger()->LogDebug(__VA_ARGS__)
#define HYPERENGINE_CLIENT_TRACE(...) ::HyperEngine::Log::GetClientLogger()->LogTrace(__VA_ARGS__)