/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#	include <HyperRendering/OpenGL33/Linux/PlatformContext.hpp>

namespace HyperRendering::OpenGL33::Linux
{
	bool CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
		m_window = static_cast<HyperPlatform::Linux::CWindow*>(window);

		GLint attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		m_visual_info = glXChooseVisual(m_window->display(), 0, attributes);
		m_graphics_context = glXCreateContext(m_window->display(), m_visual_info, nullptr, GL_TRUE);
		glXMakeCurrent(m_window->display(), m_window->window(), m_graphics_context);
	}

	void CPlatformContext::shutdown()
	{
		glXDestroyContext(m_window->display(), m_graphics_context);
	}

	void CPlatformContext::swap_buffers() const
	{
		glXSwapBuffers(m_window->display(), m_window->window());
	}
} // namespace HyperRendering::OpenGL33::Linux
#endif
