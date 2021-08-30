/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperRendering/RenderEngine.hpp"

#if HYPERENGINE_BUILD_OPENGL
#	include "HyperOpenGL/GraphicsContext.hpp"
#endif

namespace HyperRendering
{
	RenderEngine::RenderEngine(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window)
		: m_event_manager(t_event_manager)
		, m_window(t_window)
	{
	}

	auto RenderEngine::initialize() -> HyperCore::InitializeResult
	{
		switch (m_window.graphics_api())
		{
		case HyperPlatform::GraphicsApi::OpenGL:
			m_graphics_context = std::make_unique<HyperOpenGL::GraphicsContext>(m_event_manager, m_window);
			break;
		case HyperPlatform::GraphicsApi::Vulkan:
			m_graphics_context = nullptr;
			break;
		default:
			break;
		}

		if (m_graphics_context == nullptr)
		{
			HyperCore::Logger::fatal("Failed to create graphics context");
			return HyperCore::ConstructError::OutOfMemory;
		}
		
		m_graphics_context->initialize();

		return {};
	}

	auto RenderEngine::update() -> void
	{
		m_graphics_context->update();
	}
} // namespace HyperRendering
