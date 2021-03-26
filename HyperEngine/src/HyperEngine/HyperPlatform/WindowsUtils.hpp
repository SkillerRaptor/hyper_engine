#pragma once

#include <HyperCore/HyperPlatformDefines.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <string>
	#include <optional>
	
	namespace HyperEngine
	{
		namespace PlatformUtils
		{
			std::optional<std::string> SaveFile(const std::string& filter);
			std::optional<std::string> OpenFile(const std::string& filter);
		}
	}
#endif