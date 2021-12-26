/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * FIXME: Try to not include RenderContext into EngineLoop to avoid leaking of
 * vulkan headers and definitions
 */

#pragma once

#include "HyperEngine/Application.hpp"
#include "HyperEngine/Platform/Window.hpp"
#include "HyperEngine/Rendering/RenderContext.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/NonNullOwnPtr.hpp"
#include "HyperEngine/Support/OwnPtr.hpp"

#include <memory>

namespace HyperEngine
{
	class EngineLoop
	{
	public:
		explicit EngineLoop(Application &application);

		EngineLoop(EngineLoop &&other) noexcept;
		EngineLoop &operator=(EngineLoop &&other) noexcept;

		Expected<void> initialize();
		void run();

		static Expected<EngineLoop> create(Application &application);

	private:
		Application *m_application;

		OwnPtr<Window> m_window = nullptr;
		OwnPtr<RenderContext> m_render_context = nullptr;
	};
} // namespace HyperEngine
