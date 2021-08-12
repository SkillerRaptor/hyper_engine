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
		static auto launch(Application& application, int argc = 0, char** argv = nullptr) -> void;
		static auto launch(Application* application, int argc = 0, char** argv = nullptr) -> void;

	private:
		static auto launch_application(Application* application, int argc, char** argv) -> void;
	};
} // namespace HyperEngine
