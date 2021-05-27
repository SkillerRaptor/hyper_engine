#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <core/memory/sparse_memory_pool.hpp>
#include <platform/library_manager.hpp>

namespace platform::linux
{
	class library_manager : public ::platform::library_manager
	{
	private:
		struct library_data
		{
			uint32_t magic_number;
			
			std::string path;
			void* library;
		};
	
	public:
		library_manager() = default;
		virtual ~library_manager() override;
		
		virtual library_handle load(const std::string& path) override;
		virtual void unload(library_handle handle) override;
		
		virtual void* get_function(library_handle handle, const std::string& function) override;
	
	private:
		void internal_unload(library_data& data);
	
	private:
		core::sparse_memory_pool<library_data> m_storage{ 256 };
		uint32_t m_version{ 1 };
	};
}
#endif
