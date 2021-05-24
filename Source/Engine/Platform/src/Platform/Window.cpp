#include <Platform/Window.hpp>

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <Platform/Windows/WindowsWindow.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
	#include <Platform/Linux/LinuxWindow.hpp>
#endif

namespace Platform
{
	Window* Window::Construct(const WindowCreateInfo& createInfo)
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new WindowsWindow{ createInfo };
#elif HYPERENGINE_PLATFORM_LINUX
		return new LinuxWindow{ createInfo };
#endif
		return nullptr;
	}
}
