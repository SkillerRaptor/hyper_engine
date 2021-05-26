#pragma once

#define HYPERENGINE_SHARED_EXPORT
#include <platform/shared_library.hpp>

namespace rendering
{
	class HYPERENGINE_API context
	{
	public:
		virtual ~context() = default;
		
		virtual bool initialize(void* native_window) = 0;
		virtual void shutdown() = 0;
		
		virtual void update() = 0;
	};
	
	extern "C"
	{
		HYPERENGINE_API context* create_context();
	}
}
