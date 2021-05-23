#pragma once

#include <string>

namespace Platform
{
	struct LibraryHandle
	{
		uint32_t handle{ 0 };
		
		bool IsHandleValid() const;
		
		uint16_t GetIndex() const;
		uint16_t GetVersion() const;
		
		bool operator==(const LibraryHandle& libraryHandle) const;
	};
	
	class LibraryManager
	{
	public:
		virtual ~LibraryManager() = default;
		
		virtual LibraryHandle Load(const std::string& path) = 0;
		virtual void Unload(LibraryHandle libraryHandle) = 0;
		
		virtual void* GetFunction(LibraryHandle libraryHandle, const std::string& functionName) = 0;
	
		static LibraryManager* Construct();
		
	protected:
		LibraryManager() = default;
	};
}
