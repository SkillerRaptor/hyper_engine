#include <HyperCore/Log.hpp>

namespace HyperEngine
{
	Logger* Log::s_pCoreLogger{};
	Logger* Log::s_pClientLogger{};
	
	void Log::Initialize()
	{
		if (s_pCoreLogger == nullptr)
		{
			s_pCoreLogger = new Logger{ "Core", Logger::Level::Trace };
		}
		
		if (s_pClientLogger == nullptr)
		{
			s_pClientLogger = new Logger{ "Client", Logger::Level::Trace };
		}
	}
	
	void Log::Terminate()
	{
		if (s_pCoreLogger != nullptr)
		{
			delete s_pCoreLogger;
		}
		
		if (s_pClientLogger != nullptr)
		{
			delete s_pClientLogger;
		}
	}
	
	const Logger& Log::GetCoreLogger()
	{
		return *s_pCoreLogger;
	}
	
	const Logger& Log::GetClientLogger()
	{
		return *s_pClientLogger;
	}
}
