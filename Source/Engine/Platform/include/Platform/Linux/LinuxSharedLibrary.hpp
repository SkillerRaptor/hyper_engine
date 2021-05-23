#pragma once

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
	#ifdef HYPERENGINE_SHARED_EXPORT
		#define HYPERENGINE_API __attribute__((visibility("default")))
	#else
		#define HYPERENGINE_API __attribute__((visibility("hidden")))
	#endif
#endif
