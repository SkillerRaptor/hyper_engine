project "HyperEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{ 
		"src/**.cpp",
		"src/**.hpp",
		"assets/**"
	}

	includedirs
	{
		"%{wks.location}/HyperEngine/src",
		"%{wks.location}/HyperEngine/src/Hyperion",
		"%{wks.location}/HyperEngine/vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.vulkan}"
	}

	links
	{
		"HyperEngine"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "HP_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HP_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HP_DIST"
		runtime "Release"
		optimize "on"
