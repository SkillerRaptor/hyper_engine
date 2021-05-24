#pragma once

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <Core/Memory/SparseMemoryPool.hpp>
#include <Platform/LibraryManager.hpp>

namespace Platform
{
	class LinuxLibraryManager : public LibraryManager
	{
	private:
		struct LibraryData
		{
			uint16_t magicNumber;
			
			std::string path;
			void* library;
		};
	
	public:
		LinuxLibraryManager() = default;
		~LinuxLibraryManager();
		
		virtual LibraryHandle Load(const std::string& path) override;
		virtual void Unload(LibraryHandle libraryHandle) override;
		
		virtual void* GetFunction(LibraryHandle libraryHandle, const std::string& functionName) override;
	
	private:
		void InternalUnload(LibraryData& libraryData);
	
	private:
		Core::SparseMemoryPool<LibraryData> m_libraryStorage{ 256 };
		uint32_t m_libraryVersion{ 1 };
	};
}
#endif
