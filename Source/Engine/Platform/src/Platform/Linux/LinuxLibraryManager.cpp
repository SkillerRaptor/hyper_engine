#include <Platform/Linux/LinuxLibraryManager.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <dlfcn.h>

namespace Platform
{
	LinuxLibraryManager::~LinuxLibraryManager()
	{
		for (size_t i{ 0 }; i < m_libraryStorage.GetPoolSize(); i++)
		{
			InternalUnload(m_libraryStorage[static_cast<uint32_t>(i)]);
		}
		m_libraryStorage.Clear();
	}
	
	LibraryHandle LinuxLibraryManager::Load(const std::string& path)
	{
		uint32_t index{};
		LibraryData& libraryData{ m_libraryStorage.Allocate(index) };
		libraryData.magicNumber = m_libraryVersion++;
		libraryData.path = path;
		libraryData.library = dlopen(path.c_str(), RTLD_LAZY);
		
		return LibraryHandle{ (libraryData.magicNumber << 16) | index };
	}
	
	void LinuxLibraryManager::Unload(LibraryHandle libraryHandle)
	{
		LibraryData& libraryData{ m_libraryStorage[libraryHandle.GetIndex()] };
		if (libraryData.magicNumber != libraryHandle.GetVersion())
		{
			return;
		}
		
		InternalUnload(libraryData);
		m_libraryStorage.Deallocate(libraryHandle.GetIndex());
	}
	
	void* LinuxLibraryManager::GetFunction(LibraryHandle libraryHandle, const std::string& functionName)
	{
		LibraryData& libraryData{ m_libraryStorage[libraryHandle.GetIndex()] };
		if (libraryData.magicNumber != libraryHandle.GetVersion())
		{
			return nullptr;
		}
		
		return dlsym(libraryData.library, functionName.c_str());
	}
	
	void LinuxLibraryManager::InternalUnload(LinuxLibraryManager::LibraryData& libraryData)
	{
		dlclose(libraryData.library);
	}
}
#endif
