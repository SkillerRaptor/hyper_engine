#include "HyperUtilities/Log.hpp"

namespace HyperUtilities
{
	std::shared_ptr<Logger> Log::m_CoreLogger;
	std::shared_ptr<Logger> Log::m_ClientLogger;

	void Log::Init()
	{
		m_CoreLogger = std::make_shared<Logger>();
		m_CoreLogger->SetName("Core");
		m_CoreLogger->SetLevel(Logger::Level::HP_LEVEL_TRACE);

		m_ClientLogger = std::make_shared<Logger>();
		m_ClientLogger->SetName("Client");
		m_ClientLogger->SetLevel(Logger::Level::HP_LEVEL_TRACE);
	}

	void Log::Shutdown()
	{
		Logger::Shutdown();
	}

	void Log::WaitForMessage(size_t messageId)
	{
		Logger::WaitForMessage(messageId);
	}
}
