#include <editor/editor.hpp>

int main(int, char**)
{
	editor::editor* editor{ new editor::editor() };
	editor->run();
	delete editor;
}
