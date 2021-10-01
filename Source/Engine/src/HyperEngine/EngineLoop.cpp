/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/IApplication.hpp"
#include "HyperEngine/Core/Logger.hpp"

namespace HyperEngine
{
	auto CEngineLoop::create(const CEngineLoop::SDescription& description) -> bool
	{
		if (description.application == nullptr)
		{
			CLogger::fatal("CEngineLoop::create(): The description application is null");
			return false;
		}

		m_application = description.application;

		CWindow::SDescription window_description{};
		window_description.title = "HyperEditor";
		window_description.width = 1280;
		window_description.height = 720;
		window_description.window_close_callback = [this]()
		{
			m_running = false;
		};

		if (!m_window.create(window_description))
		{
			CLogger::fatal("CEngineLoop::create(): Failed to create window");
			return false;
		}

		m_running = true;

		return true;
	}

	auto CEngineLoop::run() -> void
	{
		float last_time = 0.0F;
		while (m_running)
		{
			float current_time = m_window.current_time();
			float delta_time = current_time - last_time;
			last_time = current_time;

			m_window.update();
			m_application->update();

			m_window.set_title("HyperEditor - " + std::to_string(delta_time) + "s");

			// TODO(SkillerRaptor): Rendering
		}
	}
} // namespace HyperEngine
