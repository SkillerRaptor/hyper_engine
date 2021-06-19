/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <HyperCore/Logger.hpp>
#include <HyperRendering/OpenGL33/Windows/PlatformContext.hpp>

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC,HGLRC,const int*);

namespace HyperRendering::OpenGL33::Windows
{
	void CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
		m_window = static_cast<HyperPlatform::Windows::CWindow*>(window);
		
		PIXELFORMATDESCRIPTOR pixel_format{};
		pixel_format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pixel_format.nVersion = 1;
		pixel_format.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pixel_format.iPixelType = PFD_TYPE_RGBA;
		pixel_format.cColorBits = 32;
		pixel_format.cRedBits = 8;
		pixel_format.cGreenBits = 8;
		pixel_format.cBlueBits = 8;
		pixel_format.cAlphaBits = 8;
		pixel_format.cDepthBits = 24;
		pixel_format.cStencilBits = 8;
		pixel_format.iLayerType = PFD_MAIN_PLANE;
		
		SetPixelFormat(m_window->handle(), ChoosePixelFormat(m_window->handle(), &pixel_format), &pixel_format);
		
		GLint context_attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};
		m_graphics_context = wglCreateContext(m_window->handle());
		wglMakeCurrent(m_window->handle(), m_graphics_context);
		
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
			(PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
		
		wglMakeCurrent(m_window->handle(), nullptr);
		wglDeleteContext(m_graphics_context);
		//m_graphics_context = wglCreateContext(m_window->handle());
		m_graphics_context = wglCreateContextAttribsARB(m_window->handle(), nullptr, context_attributes);
		wglMakeCurrent(m_window->handle(), m_graphics_context);
		
		HyperCore::CLogger::debug("OpenGL Info:");
		HyperCore::CLogger::debug("  Vendor: {0}", glGetString(GL_VENDOR));
		HyperCore::CLogger::debug("  Renderer: {0}", glGetString(GL_RENDERER));
		HyperCore::CLogger::debug("  Version: {0}", glGetString(GL_VERSION));
	}
	
	void CPlatformContext::shutdown()
	{
		wglDeleteContext(m_graphics_context);
	}
	
	void CPlatformContext::swap_buffers() const
	{
		SwapBuffers(m_window->handle());
	}
}
#endif
