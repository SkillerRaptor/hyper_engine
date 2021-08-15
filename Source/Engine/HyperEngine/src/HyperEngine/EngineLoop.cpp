/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/IApplication.hpp"

#include <HyperOpenGL33/Context.hpp>

namespace HyperEngine
{
	EngineLoop::EngineLoop(IApplication& application)
		: m_application(application)
		, m_window(m_application.title(), 1280, 720, m_application.graphics_api(), m_event_manager)
	{
	}
	
	EngineLoop::~EngineLoop()
	{
		delete m_render_context;
	}

	auto EngineLoop::initialize() -> HyperCore::Result<void, HyperCore::ConstructError>
	{
		m_event_manager.register_listener<HyperCore::WindowCloseEvent>(
			"EngineLoopAppCloseEvent",
			[this](const HyperCore::WindowCloseEvent&)
			{
				m_running = false;
			});
		
		auto window_result = m_window.initialize();
		if (window_result.is_error())
		{
			return window_result.error();
		}
		
		m_render_context = [this]() -> HyperRendering::IContext*
		{
			switch (m_application.graphics_api())
			{
			case HyperPlatform::GraphicsApi::OpenGL33:
				return new HyperRendering::OpenGL33::Context(m_window);
			default:
				return nullptr;
			}
		}();

		if (m_render_context == nullptr)
		{
			return HyperCore::ConstructError::UndefinedBehaviour;
		}

		auto render_context_result = m_render_context->initialize();
		if (render_context_result.is_error())
		{
			return render_context_result.error();
		}

		m_running = true;

		return {};
	}

	auto EngineLoop::run() -> void
	{
		while (m_running)
		{
			m_event_manager.process_next_event();

			m_render_context->update();

			m_window.poll_events();
		}
	}
} // namespace HyperEngine
