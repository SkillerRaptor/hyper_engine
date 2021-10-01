/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEditor/Editor.hpp"

#include <HyperEngine/Launcher.hpp>

#include <cstdio>

auto main() -> int
{
	std::setvbuf(stdout, nullptr, _IONBF, 0);
	return HyperEngine::CLauncher::launch<HyperEditor::Editor>();
}
