/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperRendering/RenderEngine.hpp"

#if HYPERENGINE_BUILD_OPENGL
#	include "HyperOpenGL/GraphicsContext.hpp"
#endif

#if HYPERENGINE_BUILD_VULKAN
#	include "HyperVulkan/GraphicsContext.hpp"
#endif

namespace HyperRendering
{
	RenderEngine::RenderEngine(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window)
		: m_event_manager(t_event_manager)
		, m_window(t_window)
	{
	}

	auto RenderEngine::initialize() -> bool
	{
		switch (m_window.graphics_api())
		{
#if HYPERENGINE_BUILD_OPENGL
		case HyperPlatform::GraphicsApi::OpenGL:
			m_graphics_context = std::make_unique<HyperOpenGL::GraphicsContext>(m_event_manager, m_window);
			break;
#endif
#if HYPERENGINE_BUILD_VULKAN
		case HyperPlatform::GraphicsApi::Vulkan:
			m_graphics_context = std::make_unique<HyperVulkan::GraphicsContext>(m_event_manager, m_window);
			break;
#endif
		default:
			break;
		}

		if (m_graphics_context == nullptr)
		{
			HyperCore::Logger::fatal("RenderEngine::initialize(): Failed to create graphics context");
			return false;
		}
		
		if (!m_graphics_context->initialize())
		{
			HyperCore::Logger::fatal("RenderEngine::initialize(): Failed to initialize graphics context");
			return false;
		}
		
		return true;
	}

	auto RenderEngine::update() -> void
	{
		m_graphics_context->update();
	}
} // namespace HyperRendering
