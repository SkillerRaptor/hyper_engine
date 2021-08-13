/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/Application.hpp"

namespace HyperEngine
{
	EngineLoop::EngineLoop(Application& application)
		: m_application(application)
		, m_window(m_application.title(), 1280, 720, m_application.graphics_api())
	{
	}
	
	auto EngineLoop::initialize() -> HyperCore::Result<void, HyperCore::ConstructError>
	{
		auto result = m_window.initialize();
		if (result.is_error())
		{
			return result.error();
		}
		
		m_event_manager.register_listener<HyperCore::WindowCloseEvent>(
			"EngineLoopAppCloseEvent",
			[this](const HyperCore::WindowCloseEvent&)
			{
				m_running = false;
			});

		m_running = true;
		
		return {};
	}

	auto EngineLoop::run() -> void
	{
		while (m_running)
		{
			m_event_manager.process_next_event();
			
			m_window.poll_events();
		}
	}
} // namespace HyperEngine
