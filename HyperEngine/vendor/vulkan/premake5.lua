project "vulkan"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/vulkan.c",
		"include/vulkan/vk_icd.h",
		"include/vulkan/vk_layer.h",
		"include/vulkan/vk_platform.h",
		"include/vulkan/vk_sdk_platform.h",
		"include/vulkan/vulkan.h",
		"include/vulkan/vulkan.hpp",
		"include/vulkan/vulkan_android.h",
		"include/vulkan/vulkan_beta.h",
		"include/vulkan/vulkan_core.h",
		"include/vulkan/vulkan_directfb.h",
		"include/vulkan/vulkan_fuchsia.h",
		"include/vulkan/vulkan_ggp.h",
		"include/vulkan/vulkan_ios.h",
		"include/vulkan/vulkan_macros.h",
		"include/vulkan/vulkan_metal.h",
		"include/vulkan/vulkan_vi.h",
		"include/vulkan/vulkan_wayland.h",
		"include/vulkan/vulkan_win32.h",
		"include/vulkan/vulkan_xcb.h",
		"include/vulkan/vulkan_xlib.h",
		"include/vulkan/vulkan_xlib_xrandr.h"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
