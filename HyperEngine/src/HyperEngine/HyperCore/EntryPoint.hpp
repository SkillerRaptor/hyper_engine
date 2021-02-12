#pragma once

#include "HyperCore/Core.hpp"

extern HyperCore::Application* HyperCore::CreateApplication();

int main(int argc, char** argv)
{
	HyperUtilities::Log::Init();

	HyperCore::Application* app = HyperCore::CreateApplication();
	app->Run();
	delete app;

	HyperUtilities::Log::Shutdown();
}
