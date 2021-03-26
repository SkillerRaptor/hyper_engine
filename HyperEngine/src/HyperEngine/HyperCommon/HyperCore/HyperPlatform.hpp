#pragma once

#include "HyperPlatformDefines.hpp"

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <HyperPlatform/WindowsUtils.hpp>
#else
	#error Platform is not supported in the moment.
#endif