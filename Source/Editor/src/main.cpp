/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperEditor/Editor.hpp>
#include <HyperEngine/Launcher.hpp>
#include <cstdint>

int32_t main(int32_t argc, char** argv)
{
	HyperEditor::CEditor editor = HyperEditor::CEditor();
	HyperEngine::CLauncher::launch(editor, argc, argv);

	return 0;
}
