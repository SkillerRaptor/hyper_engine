#include <Hyperion/HyperCore/Application.hpp>
#include <Hyperion/HyperCore/EntryPoint.hpp>

#include "SandboxLayer.hpp"

using namespace Hyperion;

class Sandbox : public Application
{
public:
	Sandbox()
		: Application("Sandbox", 1280, 720)
	{
		SetAppIcon("assets/textures/HyperEngine.PNG");

		PushLayer(new SandboxLayer());
	}

	~Sandbox()
	{
	}
};

Application* Hyperion::CreateApplication()
{
	return new Sandbox();
}
