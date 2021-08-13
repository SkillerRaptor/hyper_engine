/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Launcher.hpp"

#include "HyperEngine/IApplication.hpp"
#include "HyperEngine/EngineLoop.hpp"

namespace HyperEngine
{
	auto Launcher::launch(IApplication& application, int argc, char** argv) -> int
	{
		return Launcher::launch_application(&application, argc, argv);
	}

	auto Launcher::launch(IApplication* application, int argc, char** argv) -> int
	{
		return Launcher::launch_application(application, argc, argv);
	}

	auto Launcher::launch_application(IApplication* application, int, char**) -> int
	{
		if (application == nullptr)
		{
			return -1;
		}

		EngineLoop engine_loop(*application);
		auto result = engine_loop.initialize();
		if (result.is_error())
		{
			return -1;
		}

		application->startup();
		engine_loop.run();
		application->shutdown();
		
		return 0;
	}
} // namespace HyperEngine
