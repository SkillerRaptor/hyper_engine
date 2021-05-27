#pragma once

#include <common/context.hpp>
#include <vulkan/platform_context.hpp>

typedef struct VkInstance_T* VkInstance;

namespace rendering::vulkan
{
	class context : public ::rendering::context
	{
	public:
		virtual bool initialize(void* instance, void* native_window) override;
		virtual void shutdown() override;
		
		virtual void update() override;
	
	private:
		bool create_instance();
	
	private:
		platform_context* m_platform_context;
	
		VkInstance m_instance;
	};
}
