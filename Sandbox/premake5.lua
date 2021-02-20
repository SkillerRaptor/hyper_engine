project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	linkgroups "On"

	files 
	{ 
		"src/**.cpp",
		"src/**.hpp",
		"assets/**"
	}

	includedirs
	{
		"src",
		"%{wks.location}/HyperEngine/src",
		"%{wks.location}/HyperEngine/src/HyperEngine",
		"%{wks.location}/HyperEngine/vendor",
		"%{IncludeDir.fmt}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.json}",
		"%{IncludeDir.rttr}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.vulkan}"
	}

	links
	{
		"fmt",
		"Glad",
		"glfw",
		"glm",
		"ImGui",
		"ImGuizmo",
		"json",
		"rttr",
		"vulkan",
		"HyperEngine"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"

		links
		{
			"dl",
			"pthread"
		}

		buildoptions
		{
			"-lstdc++fs"
		}

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
