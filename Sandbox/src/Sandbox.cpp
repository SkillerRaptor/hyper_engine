#include <Hyperion/Core/Application.h>
#include <Hyperion/Core/EntryPoint.h>

class Sandbox : public Hyperion::Application
{
public:
	Sandbox()
	{
	}

	~Sandbox()
	{
	}
};

Hyperion::Application* Hyperion::CreateApplication()
{
	return new Sandbox();
}