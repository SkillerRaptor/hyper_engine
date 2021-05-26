#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <platform/windows/shared_library.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
	#include <platform/linux/shared_library.hpp>
#endif
