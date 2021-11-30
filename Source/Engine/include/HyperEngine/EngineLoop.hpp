/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Application.hpp"
#include "HyperEngine/Platform/Window.hpp"
#include "HyperEngine/Rendering/RenderContext.hpp"
#include "HyperEngine/Support/Expected.hpp"

namespace HyperEngine
{
	class EngineLoop
	{
	public:
		void run();

		static Expected<EngineLoop> create(Application &application);

	private:
		EngineLoop(
			Application &application,
			Window window,
			RenderContext render_context);

	private:
		Application &m_application;

		Window m_window;
		RenderContext m_render_context;
	};
} // namespace HyperEngine
