/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include <HyperCore/Logger.hpp>

namespace HyperEngine
{
	EngineLoop::EngineLoop(Application& application)
		: m_application(application)
	{
		HyperCore::Logger::debug("Test", 1, 2, 3);
		
		m_event_manager.register_listener<HyperCore::WindowCloseEvent>(
			"EngineLoopAppCloseEvent",
			[this](const HyperCore::WindowCloseEvent&)
			{
				m_running = false;
			});

		m_running = true;
	}

	void EngineLoop::run()
	{
		while (m_running)
		{
			m_event_manager.process_next_event();
		}
	}
} // namespace HyperEngine
