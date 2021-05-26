#pragma once

#include <common/context.hpp>

typedef struct VkInstance_T* VkInstance;

namespace rendering::vulkan
{
	class context : public ::rendering::context
	{
	public:
		virtual bool initialize(void* native_window) override;
		virtual void shutdown() override;
		
		virtual void update() override;
	
	private:
		bool create_instance();
	
	private:
		VkInstance m_instance;
	};
}
