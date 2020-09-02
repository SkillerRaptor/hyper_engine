#include <Hyperion/Core/Application.hpp>
#include <Hyperion/Core/EntryPoint.hpp>

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