#pragma once

#include <string>

namespace platform
{
	struct library_handle
	{
		uint32_t handle{ 0 };
		
		bool valid() const;
		
		uint16_t index() const;
		uint16_t version() const;
		
		bool operator==(const library_handle& library_handle) const;
	};
	
	class library_manager
	{
	public:
		virtual ~library_manager() = default;
		
		virtual library_handle load(const std::string& path) = 0;
		virtual void unload(library_handle handle) = 0;
		
		virtual void* get_function(library_handle handle, const std::string& function) = 0;
	
		static library_manager* construct();
		
	protected:
		library_manager() = default;
	};
}
