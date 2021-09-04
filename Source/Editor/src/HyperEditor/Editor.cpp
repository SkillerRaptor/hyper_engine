/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEditor/Editor.hpp"

namespace HyperEditor
{
	Editor::Editor()
		: IApplication("HyperEditor", HyperPlatform::GraphicsApi::Vulkan)
	{
	}

	auto Editor::startup() -> void
	{
		m_discord_presence.initialize();
	}

	auto Editor::shutdown() -> void
	{
	}
	
	auto Editor::update() -> void
	{
		m_discord_presence.update();
	}
} // namespace HyperEditor
