/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperEngine
{
	class IApplication;
	
	class CLauncher
	{
	public:
		static void launch(IApplication& application, int32_t argc = 0, char** argv = nullptr);
		static void launch(IApplication* application, int32_t argc = 0, char** argv = nullptr);
	
	private:
		static void launch_application(IApplication& application, int32_t argc, char** argv);
	};
}
