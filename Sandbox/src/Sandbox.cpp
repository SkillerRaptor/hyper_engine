#include <Hyperion/HyperCore/Application.hpp>
#include <Hyperion/HyperCore/EntryPoint.hpp>

#include "SandboxLayer.hpp"

class Sandbox : public Hyperion::Application
{
public:
	Sandbox()
		: Application("Sandbox", 1280, 720)
	{
		PushLayer(new SandboxLayer());
	}

	~Sandbox()
	{
	}
};

Hyperion::Application* Hyperion::CreateApplication()
{
	return new Sandbox();
}
