#include <Platform/Windows/WindowsLibraryManager.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
namespace Platform
{
	WindowsLibraryManager::~WindowsLibraryManager()
	{
		for (size_t i{ 0 }; i < m_libraryStorage.GetPoolSize(); i++)
		{
			InternalUnload(m_libraryStorage[static_cast<uint32_t>(i)]);
		}
		m_libraryStorage.Clear();
	}
	
	LibraryHandle WindowsLibraryManager::Load(const std::string& path)
	{
		uint32_t index{};
		LibraryData& libraryData{ m_libraryStorage.Allocate(index) };
		libraryData.magicNumber = m_libraryVersion++;
		libraryData.path = path;
		libraryData.library = LoadLibrary(path.c_str());
		
		return LibraryHandle{ (libraryData.magicNumber << 16) | index };
	}
	
	void WindowsLibraryManager::Unload(LibraryHandle libraryHandle)
	{
		LibraryData& libraryData{ m_libraryStorage[libraryHandle.GetIndex()] };
		if (libraryData.magicNumber != libraryHandle.GetVersion())
		{
			return;
		}
		
		InternalUnload(libraryData);
		m_libraryStorage.Deallocate(libraryHandle.GetIndex());
	}
	
	void* WindowsLibraryManager::GetFunction(LibraryHandle libraryHandle, const std::string& functionName)
	{
		LibraryData& libraryData{ m_libraryStorage[libraryHandle.GetIndex()] };
		if (libraryData.magicNumber != libraryHandle.GetVersion())
		{
			return nullptr;
		}
		
		return reinterpret_cast<void*>(GetProcAddress(libraryData.library, functionName.c_str()));
	}
	
	void WindowsLibraryManager::InternalUnload(WindowsLibraryManager::LibraryData& libraryData)
	{
		FreeLibrary(libraryData.library);
	}
}
#endif
