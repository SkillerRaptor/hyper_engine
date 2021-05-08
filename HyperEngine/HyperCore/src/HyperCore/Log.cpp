#include <HyperCore/Log.hpp>

namespace HyperCore
{
	std::shared_ptr<Logger> Log::s_pCoreLogger;
	std::shared_ptr<Logger> Log::s_pClientLogger;
	
	void Log::Initialize()
	{
		if (s_pCoreLogger == nullptr)
		{
			s_pCoreLogger = std::make_shared<Logger>("Core", Logger::Level::Trace );
		}
		
		if (s_pClientLogger == nullptr)
		{
			s_pClientLogger = std::make_shared<Logger>("Client", Logger::Level::Trace );
		}
	}
	
	const std::shared_ptr<Logger>& Log::GetCoreLogger()
	{
		return s_pCoreLogger;
	}
	
	const std::shared_ptr<Logger>& Log::GetClientLogger()
	{
		return s_pClientLogger;
	}
}
