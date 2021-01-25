#include <Hyperion/HyperCore/Application.hpp>
#include <Hyperion/HyperCore/EntryPoint.hpp>

#include "EditorLayer.hpp"

using namespace Hyperion;

class HyperEditor : public Application
{
public:
	HyperEditor()
		: Application("HyperEditor", 1280, 720)
	{
		SetAppIcon("assets/textures/HyperEngine.PNG");

		PushLayer(new EditorLayer());
	}

	~HyperEditor()
	{
	}
};

Application* Hyperion::CreateApplication()
{
	return new HyperEditor();
}
