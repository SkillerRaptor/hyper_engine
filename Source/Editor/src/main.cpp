/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEditor/Editor.hpp"

#include <HyperEngine/Launcher.hpp>

auto main() -> int
{
	return HyperEngine::CLauncher::launch<HyperEditor::Editor>();
}
