#include "Log.hpp"

namespace Hyperion
{
	Ref<Logger> Log::m_CoreLogger;
	Ref<Logger> Log::m_ClientLogger;

	void Log::Init()
	{
		m_CoreLogger = CreateRef<Logger>();
		m_CoreLogger->SetName("Core");
		m_CoreLogger->SetLevel(Logger::Level::HP_LEVEL_TRACE);

		m_ClientLogger = CreateRef<Logger>();
		m_ClientLogger->SetName("Client");
		m_ClientLogger->SetLevel(Logger::Level::HP_LEVEL_TRACE);
	}
}
