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

	auto Context::initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		glfwMakeContextCurrent(m_window.native_window());

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			glfwDestroyWindow(m_window.native_window());
			glfwTerminate();
		
			HyperCore::Logger::fatal("Failed to initialize GLAD!");
			return HyperCore::Errors::ConstructError::Incomplete;
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
} // namespace HyperRendering::OpenGL33
