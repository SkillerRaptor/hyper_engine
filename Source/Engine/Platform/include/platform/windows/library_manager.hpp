#pragma once

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <core/memory/sparse_memory_pool.hpp>
#include <platform/library_manager.hpp>
	
#include <Windows.h>

namespace platform::windows
{
	class library_manager : public ::platform::library_manager
	{
	private:
		struct library_data
		{
			uint16_t magic_number;
			
			std::string path;
			HINSTANCE library;
		};
	
	public:
		library_manager() = default;
		virtual ~library_manager() override;
		
		virtual library_handle load(const std::string& path) override;
		virtual void unload(library_handle library_handle) override;
		
		virtual void* get_function(library_handle library_handle, const std::string& function) override;
	
	private:
		void internal_unload(library_data& data);
	
	private:
		core::sparse_memory_pool<library_data> m_storage{ 256 };
		uint32_t m_version{ 1 };
	};
}
#endif
