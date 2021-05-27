#pragma once

namespace rendering
{
	class platform_context
	{
	public:
		virtual void initialize(void* instance, void* native_window) = 0;
		virtual void shutdown() = 0;
		
		virtual const char* get_required_extension() const = 0;
		
		static platform_context* construct();
	};
}
