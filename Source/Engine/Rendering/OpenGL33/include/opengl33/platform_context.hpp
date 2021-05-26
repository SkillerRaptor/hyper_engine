#pragma once

namespace rendering
{
	class platform_context
	{
	public:
		virtual void initialize() = 0;
		virtual void shutdown() = 0;
		
		virtual void swap_buffers() = 0;
		
		static platform_context* construct();
	};
}
