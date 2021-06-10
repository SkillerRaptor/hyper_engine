/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperEngine
{
	class IApplication;
	
	class CLauncher
	{
	public:
		static void launch(IApplication& application);
		static void launch(IApplication* application);
	};
}
