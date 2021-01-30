#pragma once

#include "HyperCore/Core.hpp"

extern Hyperion::Application* Hyperion::CreateApplication();

int main(int argc, char** argv)
{
	Hyperion::Log::Init();

	auto app = Hyperion::CreateApplication();
	app->Run();
	delete app;

	Hyperion::Log::Shutdown();
}
