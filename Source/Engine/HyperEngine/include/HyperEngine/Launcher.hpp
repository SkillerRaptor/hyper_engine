/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/IApplication.hpp"

namespace HyperEngine
{
	class Launcher
	{
	public:
		static auto launch(IApplication& application, int argc = 0, char** argv = nullptr) -> int;
		static auto launch(IApplication* application, int argc = 0, char** argv = nullptr) -> int;

	private:
		static auto launch_application(IApplication* application, int argc, char** argv) -> int;
	};
} // namespace HyperEngine
