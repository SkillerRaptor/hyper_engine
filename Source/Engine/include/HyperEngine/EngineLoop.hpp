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
		HYPERENGINE_NON_COPYABLE(EngineLoop);

	public:
		~EngineLoop();

		EngineLoop(EngineLoop &&other) noexcept;
		EngineLoop &operator=(EngineLoop &&other) noexcept;

		void run();

		static Expected<EngineLoop> create(Application &application);

	private:
		EngineLoop(Application *application, Error &error);

	private:
		Application *m_application = nullptr;

		Window *m_window = nullptr;
		RenderContext *m_render_context = nullptr;
	};
} // namespace HyperEngine
