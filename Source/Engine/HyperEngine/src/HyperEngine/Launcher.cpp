/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperEngine/EngineLoop.hpp>
#include <HyperEngine/IApplication.hpp>
#include <HyperEngine/Launcher.hpp>

namespace HyperEngine
{
	void CLauncher::launch(IApplication& application, int32_t argc, char** argv)
	{
		CLauncher::launch_application(application, argc, argv);
	}

	void CLauncher::launch(IApplication* application, int32_t argc, char** argv)
	{
		CLauncher::launch_application(*application, argc, argv);
	}

	void CLauncher::launch_application(IApplication& application, int32_t argc, char** argv)
	{
		CEngineLoop engine_loop;
		engine_loop.initialize(&application);

		application.startup();
		engine_loop.run();
		application.shutdown();

		engine_loop.shutdown();
	}
} // namespace HyperEngine
