#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <platform/window.hpp>

#include <X11/Xlib.h>

namespace platform::linux
{
	class window : public ::platform::window
	{
	public:
		explicit window(const window_create_info& create_info);
		
		virtual bool initialize(library_manager* library_manager) override;
		virtual void shutdown() override;
		
		virtual void update() override;
	
	private:
		Window m_window;
		Display* m_display;
		int m_screen;
	};
}
#endif
