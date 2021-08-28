/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/IApplication.hpp"

#include <chrono>

namespace HyperEngine
{
	EngineLoop::EngineLoop(IApplication& application)
		: m_application(application)
		, m_window({ m_application.title(), m_event_manager, m_application.graphics_api() })
	{
	}

	auto EngineLoop::initialize() -> HyperCore::InitializeResult
	{
		m_event_manager.register_listener<HyperCore::WindowCloseEvent>(
			[this](const HyperCore::WindowCloseEvent&)
			{
				m_running = false;
			});

		auto window_result = m_window.initialize();
		if (window_result.is_error())
		{
			HyperCore::Logger::fatal("Failed to create window");
			return window_result.error();
		}

		m_running = true;

		return {};
	}

	auto EngineLoop::terminate() -> void
	{
	}

	auto EngineLoop::run() -> void
	{
		auto last_time = 0.0F;
		while (m_running)
		{
			auto current_time = m_window.time();
			auto delta_time = last_time - current_time;
			last_time = current_time;

			(void) delta_time;

			m_event_manager.process_events();
			m_window.poll_events();
		}
	}
} // namespace HyperEngine
