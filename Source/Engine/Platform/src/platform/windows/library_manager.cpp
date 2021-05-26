#include <platform/windows/library_manager.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
namespace platform::windows
{
	library_manager::~library_manager()
	{
		for (size_t i{ 0 }; i < m_storage.size(); i++)
		{
			internal_unload(m_storage[static_cast<uint32_t>(i)]);
		}
		m_storage.clear();
	}
	
	library_handle library_manager::load(const std::string& path)
	{
		uint32_t index{};
		library_data& data{ m_storage.allocate(index) };
		data.magic_number = m_version++;
		data.path = path;
		data.library = LoadLibrary(path.c_str());
		
		return library_handle{ (data.magic_number << 16) | index };
	}
	
	void library_manager::unload(library_handle handle)
	{
		library_data& data{ m_storage[handle.index()] };
		if (data.magic_number != handle.version())
		{
			return;
		}
		
		internal_unload(data);
		m_storage.deallocate(handle.index());
	}
	
	void* library_manager::get_function(library_handle handle, const std::string& function)
	{
		library_data& data{ m_storage[handle.index()] };
		if (data.magic_number != handle.version())
		{
			return nullptr;
		}
		
		return reinterpret_cast<void*>(GetProcAddress(data.library, function.c_str()));
	}
	
	void library_manager::internal_unload(library_data& data)
	{
		FreeLibrary(data.library);
	}
}
#endif
