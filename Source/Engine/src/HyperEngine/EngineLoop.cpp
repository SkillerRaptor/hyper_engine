/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/Support/Prerequisites.hpp"

namespace HyperEngine
{
	EngineLoop::EngineLoop(
		Application &application,
		Window window,
		Rendering::RenderContext render_context)
		: m_application(application)
		, m_window(std::move(window))
		, m_render_context(std::move(render_context))
	{
	}

	void EngineLoop::run()
	{
		HYPERENGINE_UNUSED_VARIABLE(m_application);
		HYPERENGINE_UNUSED_VARIABLE(m_window);
		HYPERENGINE_UNUSED_VARIABLE(m_render_context);

		while (true)
		{
			m_window.update();
			break;
		}
	}

	Expected<EngineLoop> EngineLoop::create(Application &application)
	{
#if HYPERENGINE_DEBUG
		constexpr bool validation_layers_enabled = true;
#else
		constexpr bool validation_layers_enabled = false;
#endif
		
		Expected<Window> window = Window::create("HyperEngine", 1280, 720);
		if (window.is_error())
		{
			return window.error();
		}

		Expected<Rendering::RenderContext> render_context =
			Rendering::RenderContext::create(validation_layers_enabled);
		if (render_context.is_error())
		{
			return render_context.error();
		}

		return EngineLoop(
			application,
			std::move(window.value()),
			std::move(render_context.value()));
	}
} // namespace HyperEngine
