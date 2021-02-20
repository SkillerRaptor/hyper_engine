#include "HyperRendering/Window.hpp"

#include "HyperCore/Core.hpp"

#if defined(HP_PLATFORM_WINDOWS)
	#include "HyperSystem/HyperWindows/WindowsWindow.hpp"
#elif defined(HP_PLATFORM_LINUX)
	#include "HyperSystem/HyperLinux/LinuxWindow.hpp"
#endif

namespace HyperRendering
{
	HyperCore::Ref<Window> Window::Construct(const WindowPropsInfo& windowPropsInfo)
	{
	#if defined(HP_PLATFORM_WINDOWS)
		return HyperCore::CreateRef<HyperSystem::WindowsWindow>(windowPropsInfo);
	#elif defined(HP_PLATFORM_LINUX)
		return HyperCore::CreateRef<HyperSystem::LinuxWindow>(windowPropsInfo);
	#else
		HP_ASSERT(false, "Couldn't create the window - Unknown platform!");
	#endif
	return nullptr;
	}
}
