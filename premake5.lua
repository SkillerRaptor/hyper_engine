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
IncludeDir["GLFW"] = "%{wks.location}/HyperEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/HyperEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/HyperEngine/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/HyperEngine/vendor/ImGuizmo"
IncludeDir["stb_image"] = "%{wks.location}/HyperEngine/vendor/stb_image"

group "Dependencies"
	include "vendor/premake"
	include "HyperEngine/vendor/GLFW"
	include "HyperEngine/vendor/Glad"
	include "HyperEngine/vendor/imgui"
	include "HyperEngine/vendor/ImGuizmo"
group ""

include "HyperEngine"
include "Sandbox"
