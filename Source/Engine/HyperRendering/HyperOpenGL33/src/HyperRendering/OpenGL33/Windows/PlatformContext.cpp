/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <HyperRendering/OpenGL33/Windows/PlatformContext.hpp>

namespace HyperRendering::OpenGL33::Windows
{
	void CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
		m_window = static_cast<HyperPlatform::Windows::CWindow*>(window);
		
		HDC window_handle = GetDC(m_window->handle());
		
		PIXELFORMATDESCRIPTOR pixel_format{};
		pixel_format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pixel_format.nVersion = 1;
		pixel_format.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pixel_format.iPixelType = PFD_TYPE_RGBA;
		pixel_format.cColorBits = 32;
		pixel_format.cRedBits = 0;
		pixel_format.cRedShift = 0;
		pixel_format.cGreenBits = 0;
		pixel_format.cGreenShift = 0;
		pixel_format.cBlueBits = 0;
		pixel_format.cBlueShift = 0;
		pixel_format.cAlphaBits = 0;
		pixel_format.cAlphaShift = 0;
		pixel_format.cAccumBits = 0;
		pixel_format.cAccumRedBits = 0;
		pixel_format.cAccumGreenBits = 0;
		pixel_format.cAccumBlueBits = 0;
		pixel_format.cAccumAlphaBits = 0;
		pixel_format.cDepthBits = 24;
		pixel_format.cStencilBits = 8;
		pixel_format.cAuxBuffers = 0;
		pixel_format.iLayerType = PFD_MAIN_PLANE;
		pixel_format.bReserved = 0;
		pixel_format.dwLayerMask = 0;
		pixel_format.dwVisibleMask = 0;
		pixel_format.dwDamageMask = 0;
		
		SetPixelFormat(window_handle, ChoosePixelFormat(window_handle, &pixel_format), &pixel_format);
		
		m_graphics_context = wglCreateContext(window_handle);
		wglMakeCurrent(window_handle, m_graphics_context);
	}
	
	void CPlatformContext::shutdown()
	{
		wglDeleteContext(m_graphics_context);
	}
	
	void CPlatformContext::swap_buffers() const
	{
		wglSwapLayerBuffers(GetDC(m_window->handle()), WGL_SWAP_MAIN_PLANE);
	}
}
#endif
