/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Application.hpp"
#include "HyperEngine/Support/Expected.hpp"

namespace HyperEngine
{
	class EngineLoop
	{
	public:
		void run();

		static Expected<EngineLoop> create(Application &application);

	private:
		EngineLoop(Application &application, Error &error);

	private:
		Application &m_application;
	};
} // namespace HyperEngine
