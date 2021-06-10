/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperEditor/Editor.hpp>
#include <HyperEngine/Launcher.hpp>
#include <cstdint>

int32_t main(int32_t argc, char** argv)
{
	HyperEditor::CEditor editor(argc, argv);
	
	HyperEngine::CLauncher::launch(editor);
	
	return 0;
}
