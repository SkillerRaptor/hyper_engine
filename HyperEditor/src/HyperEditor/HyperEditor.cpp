#include "HyperEditor.hpp"

#include "EditorState.hpp"
#include "ProjectSelectionState.hpp"

namespace HyperEditor
{
	void HyperEditor::OnInitialize(ApplicationInfo& applicationInfo)
	{
		applicationInfo.szTitle = "HyperEditor";
		applicationInfo.width = 1280;
		applicationInfo.height = 720;
		applicationInfo.szAppIcon = "assets/textures/AppIcon.png";
	}
	
	void HyperEditor::OnTerminate()
	{
	}
}