/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/Logger.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

namespace HyperEngine
{
	EngineLoop::EngineLoop(Application &application)
		: m_application(&application)
	{
	}

	EngineLoop::EngineLoop(EngineLoop &&other) noexcept
		: m_application(std::exchange(other.m_application, nullptr))
		, m_window(std::move(other.m_window))
		, m_render_context(std::move(other.m_render_context))
	{
	}

	EngineLoop &EngineLoop::operator=(EngineLoop &&other) noexcept
	{
		m_application = std::exchange(other.m_application, nullptr);
		m_window = std::move(other.m_window);
		m_render_context = std::move(other.m_render_context);
		return *this;
	}

	Expected<void> EngineLoop::initialize()
	{
#if HYPERENGINE_DEBUG
		constexpr bool validation_layers_enabled = true;
#else
		constexpr bool validation_layers_enabled = false;
#endif

		auto window = Window::create("HyperEngine", 1280, 720);
		if (window.is_error())
		{
			return window.error();
		}

		m_window = std::move(window.value());

		auto render_context =
			RenderContext::create(validation_layers_enabled, *m_window);
		if (render_context.is_error())
		{
			return render_context.error();
		}

		m_render_context = std::move(render_context.value());

		return {};
	}

	void EngineLoop::run()
	{
		while (true)
		{
			m_window->poll_events();
			break;
		}
	}

	Expected<EngineLoop> EngineLoop::create(Application &application)
	{
		EngineLoop engine_loop(application);
		auto result = engine_loop.initialize();
		if (result.is_error())
		{
			return result.error();
		}

		return engine_loop;
	}
} // namespace HyperEngine
