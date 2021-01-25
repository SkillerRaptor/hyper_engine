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
		PushLayer(new SandboxLayer());
	}

	~Sandbox()
	{
	}
};1

Application* Hyperion::CreateApplication()
{
	return new Sandbox();
}
