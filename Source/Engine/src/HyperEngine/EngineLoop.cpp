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
		m_rendering_api = ERenderingApi::Best;

		CWindow::SDescription window_description = {};
		window_description.title = "HyperEditor";
		window_description.width = 1280;
		window_description.height = 720;
		window_description.rendering_api = m_rendering_api;
		window_description.window_close_callback = [this]()
		{
			m_running = false;
		};
		if (!m_window.create(window_description))
		{
			CLogger::fatal("CEngineLoop::create(): Failed to create window");
			return false;
		}
		
#if HYPERENGINE_DEBUG
		constexpr bool debug_mode = true;
#else
		constexpr bool debug_mode = false;
#endif
		
		CRenderContext::SDescription render_context_description{};
		render_context_description.window = m_window.native_window();
		render_context_description.rendering_api = m_rendering_api;
		render_context_description.debug_mode = debug_mode;
		if (!m_render_context.create(render_context_description))
		{
			CLogger::fatal("CEngineLoop::create(): Failed to create render context");
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
			const float current_time = m_window.current_time();
			const float delta_time = current_time - last_time;
			last_time = current_time;

			m_window.update();
			m_application->update();

			m_window.set_title("HyperEditor - " + std::to_string(delta_time) + "s");

			if (!m_render_context.begin_frame())
			{
				CLogger::fatal("CEngineLoop::run(): Failed to begin frame");
				break;
			}

			// TODO(SkillerRaptor): Rendering

			if (!m_render_context.end_frame())
			{
				CLogger::fatal("CEngineLoop::run(): Failed to end frame");
				break;
			}
		}
	}
} // namespace HyperEngine
