#include "WindowsUtils.hpp"

#if HYPERENGINE_PLATFORM_WINDOWS
	#define GLFW_EXPOSE_NATIVE_WIN32
	
	#include <GLFW/glfw3.h>
	#include <GLFW/glfw3native.h>
	
	#include <string>
	
	namespace HyperEngine
	{
		namespace PlatformUtils
		{
			std::optional<std::string> SaveFile(const std::string& filter)
			{
				GLFWwindow* pWindow{ nullptr };
				
				OPENFILENAMEA hOpenFileName = {};
				CHAR hFile[255] = { 0 };
				hOpenFileName.lStructSize = sizeof(OPENFILENAMEA);
				hOpenFileName.hwndOwner = glfwGetWin32Window(pWindow);
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
				GLFWwindow* pWindow{ nullptr };
				
				OPENFILENAMEA hOpenFileName{};
				CHAR hFile[255]{ 0 };
				hOpenFileName.lStructSize = sizeof(OPENFILENAMEA);
				hOpenFileName.hwndOwner = glfwGetWin32Window(pWindow);
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