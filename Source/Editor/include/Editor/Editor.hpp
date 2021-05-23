#pragma once

#include <Engine/Application.hpp>

namespace Editor
{
	class Editor : public Engine::Application
	{
	private:
		virtual void initialize() override;
		virtual void terminate() override;
	};
}
