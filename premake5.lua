include "./vendor/premake/premake_customization/solution_items.lua"

workspace "HyperEngine"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Glad"] = "%{wks.location}/HyperEngine/vendor/Glad/include"
IncludeDir["glfw"] = "%{wks.location}/HyperEngine/vendor/glfw/include"
IncludeDir["glm"] = "%{wks.location}/HyperEngine/vendor/glm"
IncludeDir["ImGui"] = "%{wks.location}/HyperEngine/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/HyperEngine/vendor/ImGuizmo"
IncludeDir["stb_image"] = "%{wks.location}/HyperEngine/vendor/stb_image"
IncludeDir["vulkan"] = "%{wks.location}/HyperEngine/vendor/vulkan/include"

LibDir = {}
LibDir["vulkan"] = "%{wks.location}/HyperEngine/vendor/vulkan/lib"

group "Dependencies"
	include "vendor/premake"
	include "HyperEngine/vendor/Glad"
	include "HyperEngine/vendor/glfw"
	include "HyperEngine/vendor/glm"
	include "HyperEngine/vendor/imgui"
	include "HyperEngine/vendor/ImGuizmo"
	include "HyperEngine/vendor/vulkan"
group ""

include "HyperEngine"
include "Sandbox"
