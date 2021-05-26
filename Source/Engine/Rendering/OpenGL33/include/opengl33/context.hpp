#pragma once

#include <common/context.hpp>

namespace rendering::opengl33
{
	class context : public ::rendering::context
	{
	public:
		virtual bool initialize() override;
		virtual void shutdown() override;
		
		virtual void update() override;
	};
}
