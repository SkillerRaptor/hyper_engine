/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Launcher.hpp"

#include "HyperEngine/EngineLoop.hpp"
#include "HyperEngine/Core/Logger.hpp"

#include <cstdlib>

namespace HyperEngine
{
	auto CLauncher::launch_application(IApplication& application) -> int
	{
		CEngineLoop::SDescription engine_loop_description{};
		engine_loop_description.application = &application;

		CEngineLoop engine_loop{};
		if (!engine_loop.create(engine_loop_description))
		{
			CLogger::fatal("CLauncher::launch_application(): Failed to create engine loop");
			return EXIT_FAILURE;
		}

		application.startup();
		engine_loop.run();
		application.shutdown();

		return EXIT_SUCCESS;
	}
} // namespace HyperEngine
