#include <HyperEngine/HyperCore/Application.hpp>
#include <HyperEngine/HyperCore/EntryPoint.hpp>

#include "HyperEditor/EditorLayer.hpp"

namespace HyperEditor
{
	class HyperEditor : public HyperCore::Application
	{
	public:
		HyperEditor()
			: Application("HyperEditor", 1280, 720)
		{
			SetAppIcon("assets/textures/HyperEngine.PNG");

			PushLayer(new EditorLayer());
		}

		~HyperEditor() = default;
	};
}

HyperCore::Application* HyperCore::CreateApplication()
{
	return new HyperEditor::HyperEditor();
}
