#pragma once

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <Platform/Window.hpp>
#include <Windows.h>

namespace Platform
{
	class WindowsWindow : public Window
	{
	public:
		explicit WindowsWindow(const WindowCreateInfo& createInfo);
		
		virtual void Shutdown() override;
		
		virtual void Update() override;
	
	private:
		HINSTANCE m_hInstance{};
		HWND m_hNativeWindow{};
	};
}
#endif
