/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Launcher.hpp"

#include "HyperEngine/Application.hpp"
#include "HyperEngine/EngineLoop.hpp"

namespace HyperEngine
{
	auto Launcher::launch(Application& application, int argc, char** argv) -> void
	{
		Launcher::launch_application(&application, argc, argv);
	}

	auto Launcher::launch(Application* application, int argc, char** argv) -> void
	{
		Launcher::launch_application(application, argc, argv);
	}

	auto Launcher::launch_application(Application* application, int, char**) -> void
	{
		if (application == nullptr)
		{
			return;
		}
		
		EngineLoop engine_loop(*application);
		application->startup();
		engine_loop.run();
		application->shutdown();
	}
} // namespace HyperEngine
