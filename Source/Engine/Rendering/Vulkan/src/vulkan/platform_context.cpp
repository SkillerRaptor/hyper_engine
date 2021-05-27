#include <vulkan/platform_context.hpp>

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <vulkan/windows/platform_context.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
	#include <vulkan/linux/platform_context.hpp>
#endif

namespace rendering
{
	platform_context* platform_context::construct()
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new windows::platform_context{};
#elif HYPERENGINE_PLATFORM_LINUX
		return new linux::platform_context{};
#endif
		return nullptr;
	}
}
