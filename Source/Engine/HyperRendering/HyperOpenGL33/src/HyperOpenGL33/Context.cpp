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
		: IContext(window)
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

		return {};
	}
	
	auto Context::update() -> void
	{
		glClearColor(1.0f, 0.0f, 1.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glfwSwapBuffers(m_window.native_window());
	}
} // namespace HyperRendering::OpenGL33
