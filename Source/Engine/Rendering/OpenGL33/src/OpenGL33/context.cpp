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
		bool context::initialize()
		{
			std::cout << "init" << std::endl;
			return true;
		}
		
		void context::shutdown()
		{
		}
		
		void context::update()
		{
		}
	}
}
