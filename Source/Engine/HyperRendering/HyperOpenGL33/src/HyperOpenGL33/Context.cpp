/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperOpenGL33/Context.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperRendering::OpenGL33
{
	Context::Context(HyperPlatform::Window& window)
		: IContext(window, new Renderer2D())
	{
	}

	auto Context::initialize() -> HyperCore::Result<void, HyperCore::ConstructError>
	{
		glfwMakeContextCurrent(m_window.native_window());

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			glfwDestroyWindow(m_window.native_window());
			glfwTerminate();
		}

		m_window.event_manager()->register_listener<HyperCore::WindowFramebufferResizeEvent>(
			"HyperRendering::OpenGL33::Context::FramebufferResizeEvent",
			[](const HyperCore::WindowFramebufferResizeEvent& event)
			{
				glViewport(0, 0, event.width, event.height);
			});

		return {};
	}

	auto Context::update() -> void
	{
		glfwSwapBuffers(m_window.native_window());
	}

	auto Context::clear_implementation(const HyperMath::Vec4f& clear_color) -> void
	{
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
} // namespace HyperRendering::OpenGL33
