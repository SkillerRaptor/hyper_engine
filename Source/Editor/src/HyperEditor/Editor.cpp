/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEditor/Editor.hpp"

namespace HyperEditor
{
	Editor::Editor()
		: IApplication("HyperEditor", HyperPlatform::GraphicsApi::OpenGL)
	{
	}

	auto Editor::startup() -> void
	{
	}

	auto Editor::shutdown() -> void
	{
	}
} // namespace HyperEditor
