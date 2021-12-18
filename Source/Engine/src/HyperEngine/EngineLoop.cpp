/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/Support/Prerequisites.hpp"

namespace HyperEngine
{
	EngineLoop::EngineLoop(Application &application, Error &error)
		: m_application(&application)
	{
#if HYPERENGINE_DEBUG
		constexpr bool validation_layers_enabled = true;
#else
		constexpr bool validation_layers_enabled = false;
#endif

		auto window = Window::create("HyperEngine", 1280, 720);
		if (window.is_error())
		{
			error = window.error();
			return;
		}

		m_window = window.value();

		auto render_context =
			RenderContext::create(validation_layers_enabled, *m_window);
		if (render_context.is_error())
		{
			error = render_context.error();
			return;
		}

		m_render_context = render_context.value();
	}

	EngineLoop::~EngineLoop()
	{
		if (m_render_context != nullptr)
		{
			delete m_render_context;
		}

		if (m_window != nullptr)
		{
			delete m_window;
		}
	}

	EngineLoop::EngineLoop(EngineLoop &&other) noexcept
		: m_application(std::exchange(other.m_application, nullptr))
		, m_window(std::exchange(other.m_window, nullptr))
		, m_render_context(std::exchange(other.m_render_context, nullptr))
	{
	}

	EngineLoop &EngineLoop::operator=(EngineLoop &&other) noexcept
	{
		m_application = std::exchange(other.m_application, nullptr);
		m_window = std::exchange(other.m_window, nullptr);
		m_render_context = std::exchange(other.m_render_context, nullptr);
		return *this;
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
		Error error = Error::success();
		EngineLoop engine_loop(application, error);
		if (error.is_error())
		{
			return error;
		}

		return engine_loop;
	}
} // namespace HyperEngine
