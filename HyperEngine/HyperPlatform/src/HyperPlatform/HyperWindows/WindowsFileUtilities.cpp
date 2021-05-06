#include <HyperPlatform/FileUtilities.hpp>

#include <HyperPlatform/PlatformDetection.hpp>

#include <Windows.h>

#if HYPERENGINE_PLATFORM_WINDOWS
	namespace HyperPlatform
	{
		namespace FileUtilities
		{
			std::optional<std::string> SaveFile(const std::string& filter)
			{
				/* TODO: Retrieving GLFW window! */
				
				OPENFILENAMEA hOpenFileName = {};
				CHAR hFile[255] = { 0 };
				hOpenFileName.lStructSize = sizeof(OPENFILENAMEA);
				//hOpenFileName.hwndOwner = glfwGetWin32Window(nullptr);
				hOpenFileName.lpstrFile = hFile;
				hOpenFileName.nMaxFile = sizeof(hFile);
				hOpenFileName.lpstrFilter = filter.c_str();
				hOpenFileName.nFilterIndex = 1;
				hOpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
				if (GetOpenFileNameA(&hOpenFileName) == TRUE)
				{
					return hOpenFileName.lpstrFile;
				}
				return std::nullopt;
			}
			
			std::optional<std::string> OpenFile(const std::string& filter)
			{
				/* TODO: Retrieving GLFW window! */
				
				OPENFILENAMEA hOpenFileName{};
				CHAR hFile[255]{ 0 };
				hOpenFileName.lStructSize = sizeof(OPENFILENAMEA);
				//hOpenFileName.hwndOwner = glfwGetWin32Window(nullptr);
				hOpenFileName.lpstrFile = hFile;
				hOpenFileName.nMaxFile = sizeof(hFile);
				hOpenFileName.lpstrFilter = filter.c_str();
				hOpenFileName.nFilterIndex = 1;
				hOpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
				if (GetSaveFileNameA(&hOpenFileName) == TRUE)
				{
					return hOpenFileName.lpstrFile;
				}
				return std::nullopt;
			}
		}
	}
#endif