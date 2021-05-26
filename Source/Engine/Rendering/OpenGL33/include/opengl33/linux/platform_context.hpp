#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <opengl33/platform_context.hpp>

namespace rendering::linux
{
	class platform_context : public ::rendering::platform_context
	{
	public:
		virtual void initialize() override;
		virtual void shutdown() override;
		
		virtual void swap_buffers() override;
	};
}
#endif
