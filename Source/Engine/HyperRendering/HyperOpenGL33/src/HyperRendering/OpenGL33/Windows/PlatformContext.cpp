/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#include <HyperCore/Logger.hpp>
#include <HyperRendering/OpenGL33/Windows/PlatformContext.hpp>

namespace HyperRendering::OpenGL33::Windows
{
	using PFNWGLCREATECONTEXTATTRIBSARBPROC =
		HGLRC(WINAPI*)(HDC, HGLRC, const int32_t*);

	bool CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
		m_window = static_cast<HyperPlatform::Windows::CWindow*>(window);

		PIXELFORMATDESCRIPTOR pixel_format_description{};
		pixel_format_description.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pixel_format_description.nVersion = 1;
		pixel_format_description.dwFlags =
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pixel_format_description.iPixelType = PFD_TYPE_RGBA;
		pixel_format_description.cColorBits = 32;
		pixel_format_description.cRedBits = 8;
		pixel_format_description.cGreenBits = 8;
		pixel_format_description.cBlueBits = 8;
		pixel_format_description.cAlphaBits = 8;
		pixel_format_description.cDepthBits = 24;
		pixel_format_description.cStencilBits = 8;
		pixel_format_description.iLayerType = PFD_MAIN_PLANE;

		int32_t pixel_format =
			ChoosePixelFormat(m_window->handle(), &pixel_format_description);
		if (pixel_format == 0)
		{
			HyperCore::CLogger::error(
				"OpenGL 3.3: failed to choose pixel format!");
			return false;
		}

		SetPixelFormat(
			m_window->handle(), pixel_format, &pixel_format_description);

		HGLRC temporary_context = wglCreateContext(m_window->handle());
		wglMakeCurrent(m_window->handle(), temporary_context);

		GLint context_attributes[] = { WGL_CONTEXT_MAJOR_VERSION_ARB,
									   3,
									   WGL_CONTEXT_MINOR_VERSION_ARB,
									   3,
									   WGL_CONTEXT_PROFILE_MASK_ARB,
									   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
									   0 };

		PROC wglCreateContextAttribsARBFunction =
			wglGetProcAddress("wglCreateContextAttribsARB");
		if (wglCreateContextAttribsARBFunction == nullptr)
		{
			HyperCore::CLogger::error(
				"OpenGL 3.3: wglCreateContextAttribsARB not found!");
			return false;
		}

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
			reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
				wglCreateContextAttribsARBFunction);

		m_graphics_context = wglCreateContextAttribsARB(
			m_window->handle(), nullptr, context_attributes);
		if (m_graphics_context == nullptr)
		{
			HyperCore::CLogger::error("OpenGL 3.3: failed to create context!");
			return false;
		}

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(temporary_context);
		wglMakeCurrent(m_window->handle(), m_graphics_context);

		HyperCore::CLogger::debug("OpenGL Info:");
		HyperCore::CLogger::debug("  Vendor: {0}", glGetString(GL_VENDOR));
		HyperCore::CLogger::debug("  Renderer: {0}", glGetString(GL_RENDERER));
		HyperCore::CLogger::debug("  Version: {0}", glGetString(GL_VERSION));

		return true;
	}

	void CPlatformContext::shutdown()
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(m_graphics_context);
	}

	void CPlatformContext::swap_buffers() const
	{
		SwapBuffers(m_window->handle());
	}
} // namespace HyperRendering::OpenGL33::Windows
