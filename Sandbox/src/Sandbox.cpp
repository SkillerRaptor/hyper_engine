#include <HyperEngine/HyperCore/Application.hpp>
#include <HyperEngine/HyperCore/EntryPoint.hpp>

#include "SandboxLayer.hpp"

class Sandbox : public HyperCore::Application
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

HyperCore::Application* HyperCore::CreateApplication()
{
	return new Sandbox();
}
