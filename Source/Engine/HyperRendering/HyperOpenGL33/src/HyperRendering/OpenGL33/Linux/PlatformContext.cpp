/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Logger.hpp>
#include <HyperRendering/OpenGL33/Linux/PlatformContext.hpp>

namespace HyperRendering::OpenGL33::Linux
{
	using PFNGLXCREATECONTEXTATTRIBSARBPROC =
		GLXContext (*)(Display*, GLXFBConfig, GLXContext, Bool, const int32_t*);

	bool CPlatformContext::initialize(HyperPlatform::IWindow* window)
	{
		m_window = static_cast<HyperPlatform::Linux::CWindow*>(window);

		GLint major_glx_version = 0;
		GLint minor_glx_version = 0;
		glXQueryVersion(
			m_window->display(), &major_glx_version, &minor_glx_version);
		if (major_glx_version <= 1 && minor_glx_version < 3)
		{
			HyperCore::CLogger::error(
				"OpenGL 3.3: GLX 1.3 or greater is required!");
			return false;
		}

		GLint attributes[] = { GLX_RENDER_TYPE,
							   GLX_RGBA_BIT,
							   GLX_RED_SIZE,
							   8,
							   GLX_GREEN_SIZE,
							   8,
							   GLX_BLUE_SIZE,
							   8,
							   GLX_ALPHA_SIZE,
							   8,
							   GLX_DEPTH_SIZE,
							   24,
							   GLX_STENCIL_SIZE,
							   8,
							   GLX_DOUBLEBUFFER,
							   True,
							   None };

		int32_t frame_buffer_config_count = 0;
		GLXFBConfig* frame_buffer_configs = glXChooseFBConfig(
			m_window->display(),
			m_window->screen(),
			attributes,
			&frame_buffer_config_count);
		if (frame_buffer_configs == nullptr)
		{
			HyperCore::CLogger::error(
				"OpenGL 3.3: failed to retrieve a framebuffer config!");
			return false;
		}

		int32_t best_frame_buffer_config_index = -1;
		int32_t worst_frame_buffer_config_index = -1;
		int32_t best_sampler_count = -1;
		int32_t worst_sampler_count = 999;

		for (int32_t i = 0; i < frame_buffer_config_count; ++i)
		{
			XVisualInfo* visual_info = glXGetVisualFromFBConfig(
				m_window->display(), frame_buffer_configs[i]);
			if (visual_info != nullptr)
			{
				int32_t buffer_sampler;
				int32_t samples;
				glXGetFBConfigAttrib(
					m_window->display(),
					frame_buffer_configs[i],
					GLX_SAMPLE_BUFFERS,
					&buffer_sampler);
				glXGetFBConfigAttrib(
					m_window->display(),
					frame_buffer_configs[i],
					GLX_SAMPLES,
					&samples);

				if (best_frame_buffer_config_index < 0 ||
					(buffer_sampler && samples > best_sampler_count))
				{
					best_frame_buffer_config_index = i;
					best_sampler_count = samples;
				}

				if (worst_frame_buffer_config_index < 0 || !buffer_sampler ||
					samples < worst_sampler_count)
				{
					worst_frame_buffer_config_index = i;
					worst_sampler_count = samples;
				}
			}

			XFree(visual_info);
		}

		GLXFBConfig best_frame_buffer_config =
			frame_buffer_configs[best_frame_buffer_config_index];
		XFree(frame_buffer_configs);

		m_visual_info = glXGetVisualFromFBConfig(
			m_window->display(), best_frame_buffer_config);

		__GLXextFuncPtr glXCreateContextAttribsARBFunction =
			glXGetProcAddressARB(
				reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB"));
		if (glXCreateContextAttribsARBFunction == nullptr)
		{
			HyperCore::CLogger::error(
				"OpenGL 3.3: glXCreateContextAttribsARB not found!");
			return false;
		}

		PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB =
			reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(
				glXCreateContextAttribsARBFunction);

		GLint context_attributes[] = { GLX_CONTEXT_MAJOR_VERSION_ARB,
									   3,
									   GLX_CONTEXT_MINOR_VERSION_ARB,
									   3,
									   GLX_CONTEXT_FLAGS_ARB,
									   GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
									   None };

		m_graphics_context = glXCreateContextAttribsARB(
			m_window->display(),
			best_frame_buffer_config,
			nullptr,
			GL_TRUE,
			context_attributes);
		glXMakeCurrent(
			m_window->display(), m_window->window(), m_graphics_context);

		HyperCore::CLogger::debug("OpenGL Info:");
		HyperCore::CLogger::debug("  Vendor: {0}", glGetString(GL_VENDOR));
		HyperCore::CLogger::debug("  Renderer: {0}", glGetString(GL_RENDERER));
		HyperCore::CLogger::debug("  Version: {0}", glGetString(GL_VERSION));

		return true;
	}

	void CPlatformContext::shutdown()
	{
		glXDestroyContext(m_window->display(), m_graphics_context);
	}

	void CPlatformContext::swap_buffers() const
	{
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glXSwapBuffers(m_window->display(), m_window->window());
	}
} // namespace HyperRendering::OpenGL33::Linux
