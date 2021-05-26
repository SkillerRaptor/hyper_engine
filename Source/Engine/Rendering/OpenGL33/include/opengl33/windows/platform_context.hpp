#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <opengl33/platform_context.hpp>

#include <Windows.h>
#include <GL/gl.h>

namespace rendering::windows
{
	class platform_context : public ::rendering::platform_context
	{
	public:
		virtual void initialize(void* instance, void* native_window) override;
		virtual void shutdown() override;
		
		virtual void swap_buffers() override;
		
	private:
		HINSTANCE m_instance;
		HWND m_window;
		
		HGLRC m_context;
	};
}
#endif
