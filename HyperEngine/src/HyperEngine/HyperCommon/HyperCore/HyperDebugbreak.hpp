#pragma once

#include "HyperPlatformDefines.hpp"

#if HYPERENGINE_DEBUG
	#if HYPERENGINE_PLATFORM_WINDOWS
		#define HYPERENGINE_DEBUGBREAK() __debugbreak()
	#elif HYPERENGINE_PLATFORM_LINUX
		#include <signal.h>
		#define HYPERENGINE_DEBUGBREAK() raise(SIGTRAP)
	#endif
#else
	#define HYPERENGINE_DEBUGBREAK()
#endif