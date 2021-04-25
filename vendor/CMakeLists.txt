#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#-------------------------------------------------------------------------------------------
# assimp
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(ASSIMP_HUNTER_ENABLED OFF)
hyperengine_set_cache_value(BUILD_SHARED_LIBS OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_FRAMEWORK OFF)
hyperengine_set_cache_value(ASSIMP_DOUBLE_PRECISION OFF)
hyperengine_set_cache_value(ASSIMP_OPT_BUILD_PACKAGES OFF)
hyperengine_set_cache_value(ASSIMP_ANDROID_JNIIOSYSTEM OFF)
hyperengine_set_cache_value(ASSIMP_NO_EXPORT OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_ZLIB OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_ASSIMP_TOOLS ON)
hyperengine_set_cache_value(ASSIMP_BUILD_SAMPLES OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_TESTS OFF)
hyperengine_set_cache_value(ASSIMP_COVERALLS OFF)
hyperengine_set_cache_value(ASSIMP_INSTALL OFF)
hyperengine_set_cache_value(ASSIMP_ERROR_MAX OFF)
hyperengine_set_cache_value(ASSIMP_ASAN OFF)
hyperengine_set_cache_value(ASSIMP_UBSAN OFF)
hyperengine_set_cache_value(ASSIMP_BUILD_DOCS OFF)
hyperengine_set_cache_value(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
hyperengine_set_cache_value(ASSIMP_IGNORE_GIT_HASH OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/assimp)
set_target_properties(assimp PROPERTIES FOLDER vendor/assimp)

#-------------------------------------------------------------------------------------------
# DirectXShaderCompiler
#-------------------------------------------------------------------------------------------
if (WIN32)
    hyperengine_set_cache_value(DXC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/DirectXShaderCompiler/DirectXShaderCompiler.zip")
    hyperengine_set_cache_value(DXC_URL "https://ci.appveyor.com/api/projects/dnovillo/directxshadercompiler/artifacts/build%2FRelease%2Fdxc-artifacts.zip?branch=master&pr=false&job=image%3A%20Visual%20Studio%202019")
elseif (UNIX AND NOT APPLE)
    hyperengine_set_cache_value(DXC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/DirectXShaderCompiler/DirectXShaderCompiler.tar.gz")
    hyperengine_set_cache_value(DXC_URL "https://ci.appveyor.com/api/projects/dnovillo/directxshadercompiler/artifacts/build%2Fdxc-artifacts.tar.gz?branch=master&pr=false&job=image%3A%20Ubuntu")
endif ()

if (NOT EXISTS "${DXC_FILE}")
    file(DOWNLOAD ${DXC_URL} ${DXC_FILE})

    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf
            ${DXC_FILE} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/DirectXShaderCompiler/)
endif ()

add_library(dxcompiler SHARED IMPORTED GLOBAL)
if (WIN32)
    set_target_properties(
            dxcompiler
            PROPERTIES
            IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/vendor/DirectXShaderCompiler/bin/dxcompiler.dll
            IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/vendor/DirectXShaderCompiler/lib/dxcompiler.lib)
elseif (UNIX AND NOT APPLE)
    set_target_properties(
            dxcompiler
            PROPERTIES
            IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/vendor/DirectXShaderCompiler/lib/lixdxcompiler.so)
endif ()

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(FMT_PEDANTIC OFF)
hyperengine_set_cache_value(FMT_WERROR OFF)
hyperengine_set_cache_value(FMT_DOC OFF)
hyperengine_set_cache_value(FMT_INSTALL OFF)
hyperengine_set_cache_value(FMT_TEST OFF)
hyperengine_set_cache_value(FMT_FUZZ OFF)
hyperengine_set_cache_value(FMT_CUDA_TEST OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fmt)
set_target_properties(fmt PROPERTIES FOLDER vendor/fmt)

#-------------------------------------------------------------------------------------------
# glad
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)
set_target_properties(glad PROPERTIES FOLDER vendor/glad)

#-------------------------------------------------------------------------------------------
# glfw
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(GLFW_BUILD_EXAMPLES OFF)
hyperengine_set_cache_value(GLFW_BUILD_TESTS OFF)
hyperengine_set_cache_value(GLFW_BUILD_DOCS OFF)
hyperengine_set_cache_value(GLFW_INSTALL OFF)
hyperengine_set_cache_value(GLFW_VULKAN_STATIC OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set_target_properties(glfw PROPERTIES FOLDER vendor/glfw)

#-------------------------------------------------------------------------------------------
# glm
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glm)

#-------------------------------------------------------------------------------------------
# Dear ImGui
#-------------------------------------------------------------------------------------------
file(GLOB "IMGUI_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp")
file(GLOB "IMGUI_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h")

add_library(imgui ${IMGUI_SOURCES} ${IMGUI_HEADERS})
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(imgui PROPERTIES FOLDER vendor/imgui)

#-------------------------------------------------------------------------------------------
# ImGuizmo
#-------------------------------------------------------------------------------------------
file(GLOB "IMGUIZMO_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.cpp")
file(GLOB "IMGUIZMO_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.h")

add_library(ImGuizmo ${IMGUIZMO_SOURCES} ${IMGUIZMO_HEADERS})
target_include_directories(ImGuizmo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imgui/")
set_target_properties(ImGuizmo PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ImGuizmo PROPERTIES FOLDER vendor/ImGuizmo)

#-------------------------------------------------------------------------------------------
# json
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(JSON_BuildTests OFF)
hyperengine_set_cache_value(JSON_Install OFF)
hyperengine_set_cache_value(JSON_MultipleHeaders OFF)
hyperengine_set_cache_value(JSON_ImplicitConversions ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/json)

#-------------------------------------------------------------------------------------------
# meshoptimizer
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(MESHOPT_BUILD_DEMO OFF)
hyperengine_set_cache_value(MESHOPT_BUILD_GLTFPACK OFF)
hyperengine_set_cache_value(MESHOPT_BUILD_SHARED_LIBS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/meshoptimizer)
set_target_properties(meshoptimizer PROPERTIES FOLDER vendor/meshoptimizer)

#-------------------------------------------------------------------------------------------
# rttr
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(BUILD_RTTR_DYNAMIC OFF)
hyperengine_set_cache_value(BUILD_UNIT_TESTS OFF)
hyperengine_set_cache_value(BUILD_STATIC ON)
hyperengine_set_cache_value(BUILD_WITH_RTTI OFF)
hyperengine_set_cache_value(BUILD_BENCHMARKS OFF)
hyperengine_set_cache_value(BUILD_EXAMPLES OFF)
hyperengine_set_cache_value(BUILD_DOCUMENTATION OFF)
hyperengine_set_cache_value(BUILD_INSTALLER OFF)
hyperengine_set_cache_value(BUILD_PACKAGE OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rttr)
set_target_properties(rttr_core_lib PROPERTIES FOLDER vendor/rttr_core_lib)

#-------------------------------------------------------------------------------------------
# SPIRV-Cross
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS ON)
hyperengine_set_cache_value(SPIRV_CROSS_SHARED OFF)
hyperengine_set_cache_value(SPIRV_CROSS_STATIC ON)
hyperengine_set_cache_value(SPIRV_CROSS_CLI OFF)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_TESTS OFF)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_GLSL ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_HLSL ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_MSL ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_CPP ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_REFLECT ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_C_API ON)
hyperengine_set_cache_value(SPIRV_CROSS_ENABLE_UTIL ON)
hyperengine_set_cache_value(SPIRV_CROSS_SANITIZE_ADDRESS OFF)
hyperengine_set_cache_value(SPIRV_CROSS_SANITIZE_MEMORY OFF)
hyperengine_set_cache_value(SPIRV_CROSS_SANITIZE_THREADS OFF)
hyperengine_set_cache_value(SPIRV_CROSS_SANITIZE_UNDEFINED OFF)
hyperengine_set_cache_value(SPIRV_CROSS_NAMESPACE_OVERRIDE ON)
hyperengine_set_cache_value(SPIRV_CROSS_FORCE_STL_TYPES OFF)
hyperengine_set_cache_value(SPIRV_CROSS_SKIP_INSTALL ON)
hyperengine_set_cache_value(SPIRV_CROSS_WERROR OFF)
hyperengine_set_cache_value(SPIRV_CROSS_MISC_WARNINGS OFF)
hyperengine_set_cache_value(SPIRV_CROSS_FORCE_PIC OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Cross)
set_target_properties(spirv-cross-core PROPERTIES FOLDER vendor/spirv-cross-core)
set_target_properties(spirv-cross-cpp PROPERTIES FOLDER vendor/spirv-cross-cpp)
set_target_properties(spirv-cross-hlsl PROPERTIES FOLDER vendor/spirv-cross-hlsl)
set_target_properties(spirv-cross-glsl PROPERTIES FOLDER vendor/spirv-cross-glsl)
set_target_properties(spirv-cross-reflect PROPERTIES FOLDER vendor/spirv-cross-reflect)

#-------------------------------------------------------------------------------------------
# stb_image
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stb_image)
set_target_properties(stb_image PROPERTIES FOLDER vendor/stb_image)
