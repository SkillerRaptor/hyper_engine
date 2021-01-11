#pragma once

extern Hyperion::Application* Hyperion::CreateApplication();

int main(int argc, char** argv)
{
	auto application = Hyperion::CreateApplication();
	application->Run();
	delete application;
}
