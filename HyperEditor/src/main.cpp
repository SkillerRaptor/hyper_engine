#include <Hyperion/HyperCore/Application.hpp>
#include <Hyperion/HyperCore/EntryPoint.hpp>

#include "EditorLayer.hpp"

class HyperEditor : public Hyperion::Application
{
public:
	HyperEditor()
	{
		PushLayer(new EditorLayer());
	}

	~HyperEditor()
	{
	}
};

Hyperion::Application* Hyperion::CreateApplication()
{
	return new HyperEditor();
}
