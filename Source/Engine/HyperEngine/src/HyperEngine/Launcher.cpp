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
		auto result = engine_loop.initialize();
		if (result.is_error())
		{
			HyperCore::Logger::fatal("Failed to initialize engine loop - {}", result.error());
			return EXIT_FAILURE;
		}

		application.startup();
		engine_loop.run();
		application.shutdown();
		
		return 0;
	}
} // namespace HyperEngine
