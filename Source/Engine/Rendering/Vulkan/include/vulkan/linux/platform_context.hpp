#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <vulkan/platform_context.hpp>

#include <X11/Xlib.h>

namespace rendering::linux
{
	class platform_context : public ::rendering::platform_context
	{
	public:
		virtual void initialize(void* instance, void* native_window) override;
		virtual void shutdown() override;
		
		virtual const char* get_required_extension() const override;
	
	private:
		Display* m_display;
		Window m_window;
	};
}
#endif
