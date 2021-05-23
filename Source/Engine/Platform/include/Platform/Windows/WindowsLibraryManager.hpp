#pragma once

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <Core/Memory/SparseMemoryPool.hpp>
	#include <Platform/LibraryManager.hpp>
	
	#include <Windows.h>
	
	namespace Platform
	{
		class WindowsLibraryManager : public LibraryManager
		{
		private:
			struct LibraryData
			{
				uint16_t magicNumber;
				
				std::string path;
				HINSTANCE library;
			};
			
		public:
			WindowsLibraryManager() = default;
			~WindowsLibraryManager();
			
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
