#include "HyperSystem/HyperLinux/LinuxPlatformUtils.hpp"

#ifdef HP_PLATFORM_LINUX

#include <GLFW/glfw3.h>

#include "HyperCore/Application.hpp"

namespace HyperSystem
{
	std::optional<std::string> LinuxPlatformUtils::OpenFile(const char* filter)
	{
	#if defined(HP_PLATFORM_LINUX)

	#endif
		return std::nullopt;
	}

	std::optional<std::string> LinuxPlatformUtils::SaveFile(const char* filter)
	{
	#if defined(HP_PLATFORM_LINUX)

	#endif
		return std::nullopt;
	}
}

#endif
