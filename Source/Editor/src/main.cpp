/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEditor/Editor.hpp"

#include <HyperEngine/Launcher.hpp>

int main(int argc, char** argv)
{
	HyperEditor::Editor editor;
	HyperEngine::Launcher::launch(editor, argc, argv);

	return 0;
}
