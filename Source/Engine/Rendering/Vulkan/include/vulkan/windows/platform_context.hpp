#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <vulkan/platform_context.hpp>

#include <Windows.h>

namespace rendering::windows
{
	class platform_context : public ::rendering::platform_context
	{
	public:
		virtual void initialize(void* instance, void* native_window) override;
		virtual void shutdown() override;
		
		virtual const char* get_required_extension() const override;
		
	private:
		HINSTANCE m_instance;
		HWND m_window;
	};
}
#endif
