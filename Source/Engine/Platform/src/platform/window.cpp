#include <platform/window.hpp>

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <platform/windows/window.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#include <platform/linux/window.hpp>
#endif

namespace platform
{
	window* window::construct(const window_create_info& create_info)
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new windows::window{ create_info };
#elif HYPERENGINE_PLATFORM_LINUX
		return new linux::window{ create_info };
#else
		return nullptr;
#endif
	}
}
