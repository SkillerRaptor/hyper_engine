#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <opengl33/platform_context.hpp>

#include <X11/Xlib.h>
#include <GL/glx.h>

namespace rendering::linux
{
	class platform_context : public ::rendering::platform_context
	{
	public:
		virtual void initialize(void* instance, void* native_window) override;
		virtual void shutdown() override;
		
		virtual void swap_buffers() override;
	
	private:
		Display* m_display;
		Window m_window;
		
		GLXContext m_context;
		XVisualInfo* m_visual_info;
	};
}
#endif
