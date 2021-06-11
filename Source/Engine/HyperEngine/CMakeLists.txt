#-------------------------------------------------------------------------------------------
# Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
set(HYPERENGINE_COMMON_PATH ${CMAKE_SOURCE_DIR}/Source/Engine/HyperRendering/HyperCommon)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperEngine STATIC ${SOURCES} ${HEADERS})
target_include_directories(HyperEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${HYPERENGINE_COMMON_PATH}/src)
target_link_libraries(HyperEngine PUBLIC HyperAudio HyperCore HyperMath HyperNetwork HyperPhysics HyperPlatform HyperResource)

#-------------------------------------------------------------------------------------------
# DirectX
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    add_dependencies(HyperEngine HyperDirectX11)
    add_dependencies(HyperEngine HyperDirectX12)
endif ()

#-------------------------------------------------------------------------------------------
# OpenGL
#-------------------------------------------------------------------------------------------
find_package(OpenGL)
if (OPENGL_FOUND)
    add_dependencies(HyperEngine HyperOpenGL33)
    add_dependencies(HyperEngine HyperOpenGL46)
endif ()

#-------------------------------------------------------------------------------------------
# Vulkan
#-------------------------------------------------------------------------------------------
find_package(Vulkan)
if (Vulkan_FOUND)
    add_dependencies(HyperEngine HyperVulkan)
endif ()
