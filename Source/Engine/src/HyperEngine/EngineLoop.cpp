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
		std::unique_ptr<Window> window)
		: m_application(application)
		, m_window(std::move(window))
	{
		HYPERENGINE_UNUSED_VARIABLE(m_application);
	}

	void EngineLoop::run()
	{
		while (true)
		{
			m_window->update();
			break;
		}
	}

	Expected<EngineLoop> EngineLoop::create(Application &application)
	{
		auto window = Window::create("HyperEngine", 1280, 720);
		if (window.is_error())
		{
			return window.error();
		}

		return EngineLoop(application, std::move(window.value()));
	}
} // namespace HyperEngine
