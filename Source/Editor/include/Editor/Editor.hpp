#pragma once

#include <Engine/Application.hpp>

namespace Editor
{
	class Editor : public Engine::Application
	{
	private:
		virtual void Initialize() override;
		virtual void Terminate() override;
	};
}
