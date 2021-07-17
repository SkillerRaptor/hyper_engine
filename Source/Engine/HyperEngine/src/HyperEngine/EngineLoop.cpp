/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Events/WindowEvents.hpp>
#include <HyperEngine/EngineLoop.hpp>
#include <HyperPlatform/Window.hpp>

namespace HyperEngine
{
	void CEngineLoop::initialize(IApplication* application)
	{
		m_application = application;

		HyperPlatform::SWindowCreateInfo create_info{};
		create_info.title = "HyperEngine";
		create_info.width = 1280;
		create_info.height = 720;
		create_info.event_manager = &m_event_manager;

		m_window.initialize(create_info);
		
		m_event_manager.register_listener<HyperCore::SWindowCloseEvent>(
			"EngineLoopAppCloseEvent",
			[this](const HyperCore::SWindowCloseEvent&)
			{
				m_running = false;
			});

		m_running = true;
	}

	void CEngineLoop::shutdown()
	{
		m_window.shutdown();
	}

	void CEngineLoop::run()
	{
		while (m_running)
		{
			m_window.poll_events();
			m_event_manager.process_next_event();
		}
	}
} // namespace HyperEngine
