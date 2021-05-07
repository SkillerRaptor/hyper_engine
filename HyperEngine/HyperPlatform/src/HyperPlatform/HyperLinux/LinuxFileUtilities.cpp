#include <HyperPlatform/FileUtilities.hpp>

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
	namespace HyperEngine
	{
		namespace FileUtilities
		{
			std::optional<std::string> SaveFile(const std::string& filter)
			{
				return std::nullopt;
			}
			
			std::optional<std::string> OpenFile(const std::string& filter)
			{
				return std::nullopt;
			}
		}
	}
#endif
