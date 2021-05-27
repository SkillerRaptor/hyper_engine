#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#ifdef HYPERENGINE_SHARED_EXPORT
		#define HYPERENGINE_API __declspec(dllexport)
	#else
		#define HYPERENGINE_API __declspec(dllimport)
	#endif
#endif