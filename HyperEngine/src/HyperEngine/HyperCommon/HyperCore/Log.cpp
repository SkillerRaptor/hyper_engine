#include "Log.hpp"

namespace HyperEngine
{
	std::shared_ptr<Logger> Log::s_pCoreLogger{ nullptr };
	std::shared_ptr<Logger> Log::s_pClientLogger{ nullptr };

	void Log::Init()
	{
		if (s_pCoreLogger == nullptr)
		{
			s_pCoreLogger = std::make_shared<Logger>("Core", Logger::Level::Trace);
		}
		
		if (s_pClientLogger == nullptr)
		{
			s_pClientLogger = std::make_shared<Logger>("Client", Logger::Level::Trace);
		}
	}
}