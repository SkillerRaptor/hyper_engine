#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <platform/window.hpp>

#include <Windows.h>

namespace platform::windows
{
	class window : public ::platform::window
	{
	public:
		explicit window(const window_create_info& create_info);
		
		virtual bool initialize(library_manager* library_manager) override;
		virtual void shutdown() override;
		
		virtual void update() override;
	
	private:
		HINSTANCE m_instance{};
		HWND m_native_window{};
	};
}
#endif
