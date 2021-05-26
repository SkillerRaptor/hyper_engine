#pragma once

#include <engine/application.hpp>

namespace editor
{
	class editor : public engine::application
	{
	private:
		virtual void initialize() override;
		virtual void terminate() override;
	};
}
