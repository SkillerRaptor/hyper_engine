/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Launcher.hpp"

#include "HyperEngine/EngineLoop.hpp"

namespace HyperEngine
{
	auto Launcher::launch_application(IApplication& application) -> int
	{
		auto engine_loop = EngineLoop(application);
		if (!engine_loop.initialize())
		{
			HyperCore::Logger::fatal("Launcher::launch_application(): Failed to initialize engine loop");
			return EXIT_FAILURE;
		}

		application.startup();
		engine_loop.run();
		application.shutdown();
		
		return 0;
	}
} // namespace HyperEngine
