#include <opengl33/context.hpp>

#include <iostream>

namespace rendering
{
	HYPERENGINE_API context* create_context()
	{
		return new opengl33::context{};
	}
	
	namespace opengl33
	{
		bool context::initialize(void* instance, void* native_window)
		{
			m_platform_context = platform_context::construct();
			m_platform_context->initialize(instance, native_window);
			return true;
		}
		
		void context::shutdown()
		{
		}
		
		void context::update()
		{
			m_platform_context->swap_buffers();
		}
	}
}
