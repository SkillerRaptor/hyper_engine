#include "Editor/Editor.hpp"

int32_t main(int32_t argc, char** argv)
{
	Editor::Editor* editor = new Editor::Editor(argc, argv);
	editor->run();
	delete editor;
	
	return 0;
}
