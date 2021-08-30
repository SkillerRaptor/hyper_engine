/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperOpenGL/GraphicsContext.hpp"

#include <HyperCore/Logger.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperRendering::HyperOpenGL
{
	GraphicsContext::GraphicsContext(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window)
		: IGraphicsContext(t_event_manager, t_window)
	{
	}
	
	GraphicsContext::~GraphicsContext()
	{
		HyperCore::Logger::info("Successfully destroyed OpenGL context");
	}

	auto GraphicsContext::initialize() -> HyperCore::InitializeResult
	{
		glfwMakeContextCurrent(m_window.native_window());

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			glfwDestroyWindow(m_window.native_window());
			glfwTerminate();

			HyperCore::Logger::fatal("Failed to initialize GLAD");
			return HyperCore::ConstructError::Incomplete;
		}

		HyperCore::Logger::debug("GLAD was initialized");

		m_event_manager.register_listener<HyperGame::WindowFramebufferResizeEvent>(
			[](const HyperGame::WindowFramebufferResizeEvent& window_framebuffer_resize_event)
			{
				glViewport(0, 0, window_framebuffer_resize_event.width, window_framebuffer_resize_event.height);
			});

		HyperCore::Logger::info("Successfully created OpenGL context");

		return {};
	}

	auto GraphicsContext::update() -> void
	{
		glfwSwapBuffers(m_window.native_window());
	}
} // namespace HyperRendering::HyperOpenGL
