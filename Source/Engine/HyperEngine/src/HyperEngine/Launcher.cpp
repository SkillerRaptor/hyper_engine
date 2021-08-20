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
			return EXIT_FAILURE;
		}

		application.startup();
		engine_loop.run();
		application.shutdown();
		
		engine_loop.terminate();
		
		return 0;
	}
} // namespace HyperEngine
