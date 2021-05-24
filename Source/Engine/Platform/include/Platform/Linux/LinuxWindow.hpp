#pragma once

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <Platform/Window.hpp>

#include <X11/Xlib.h>

namespace Platform
{
	class LinuxWindow : public Window
	{
	public:
		explicit LinuxWindow(const WindowCreateInfo& createInfo);
	
		virtual void Shutdown() override;
	
		virtual void Update() override;
	
	private:
		::Window m_window;
		Display* m_display;
		int m_screen;
	};
}
#endif
