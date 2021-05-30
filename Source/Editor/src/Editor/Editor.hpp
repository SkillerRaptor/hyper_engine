#pragma once

#include <Engine/Application.hpp>

namespace Editor
{
	class Editor : public Engine::Application
	{
	public:
		Editor(int32_t argc, char** argv);
		
	private:
		virtual void startup() override;
		virtual void shutdown() override;
	};
}
