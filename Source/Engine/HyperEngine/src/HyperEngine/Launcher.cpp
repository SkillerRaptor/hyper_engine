/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperEngine/Launcher.hpp>

namespace HyperEngine
{
	void CLauncher::launch(IApplication& application)
	{
	
	}
	
	void CLauncher::launch(IApplication* application)
	{
		CLauncher::launch(*application);
	}
}
