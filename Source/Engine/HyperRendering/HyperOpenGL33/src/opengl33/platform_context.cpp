#include <opengl33/platform_context.hpp>

#include <platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <opengl33/windows/platform_context.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#include <opengl33/linux/platform_context.hpp>
#endif

namespace rendering
{
	platform_context* platform_context::construct()
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new windows::platform_context{};
#elif HYPERENGINE_PLATFORM_LINUX
		return new linux::platform_context{};
#else
		return nullptr;
#endif
	}
}
