#pragma once

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <Platform/Windows/WindowsSharedLibrary.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
	#include <Platform/Linux/LinuxSharedLibrary.hpp>
#endif
