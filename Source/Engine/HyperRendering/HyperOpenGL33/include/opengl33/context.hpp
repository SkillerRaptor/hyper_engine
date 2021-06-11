#pragma once

#include <common/context.hpp>
#include <opengl33/platform_context.hpp>

namespace rendering::opengl33
{
	class context : public ::rendering::context
	{
	public:
		virtual bool initialize(void* instance, void* native_window) override;
		virtual void shutdown() override;
		
		virtual void update() override;
	
	private:
		platform_context* m_platform_context;
	};
}
