#include "Window.hpp"

#include "HyperCore/Core.hpp"

#if defined(HP_PLATFORM_WINDOWS)
#include "Platform/OS/Windows/WindowsWindow.hpp"
#endif

namespace Hyperion
{
	Ref<Window> Window::Construct(const WindowPropsInfo& windowPropsInfo)
	{
	#if defined(HP_PLATFORM_WINDOWS)
		return CreateRef<WindowsWindow>(windowPropsInfo);
	#else
		HP_CORE_ASSERT(false, "Unknown Platform!");
		return nullptr;
	#endif
	}
}
