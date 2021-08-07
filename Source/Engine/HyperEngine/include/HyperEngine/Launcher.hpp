/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Application.hpp"

namespace HyperEngine
{
	class Application;

	class Launcher
	{
	public:
		static void launch(Application& application, int argc = 0, char** argv = nullptr);
		static void launch(Application* application, int argc = 0, char** argv = nullptr);

	private:
		static void launch_application(Application* application, int argc, char** argv);
	};
} // namespace HyperEngine
