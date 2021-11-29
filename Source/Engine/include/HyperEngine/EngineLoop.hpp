/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Application.hpp"
#include "HyperEngine/Platform/Window.hpp"
#include "HyperEngine/Support/Expected.hpp"

namespace HyperEngine
{
	class EngineLoop
	{
	public:
		EngineLoop(Application &application, std::unique_ptr<Window> window);

		void run();

		static Expected<EngineLoop> create(Application &application);

	private:
		Application &m_application;
		std::unique_ptr<Window> m_window = nullptr;
	};
} // namespace HyperEngine
