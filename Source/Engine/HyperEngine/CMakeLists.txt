#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
set(HYPERENGINE_COMMON_PATH ${CMAKE_SOURCE_DIR}/Source/Engine/HyperRendering/HyperCommon)

set(SOURCES
        src/HyperEngine/EngineLoop.cpp
        src/HyperEngine/Launcher.cpp)

set(HEADERS
        src/HyperEngine/EngineLoop.hpp
        src/HyperEngine/IApplication.hpp
        src/HyperEngine/Launcher.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperEngine STATIC ${SOURCES} ${HEADERS})
target_include_directories(HyperEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${HYPERENGINE_COMMON_PATH}/src)
target_include_directories(HyperEngine PUBLIC ${HYPERENGINE_COMMON_PATH}/src)
target_link_libraries(HyperEngine PUBLIC HyperCore)
target_link_libraries(HyperEngine PUBLIC HyperMath)
target_link_libraries(HyperEngine PUBLIC HyperPlatform)
target_link_libraries(HyperEngine PUBLIC HyperResource)

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
if (${HYPERENGINE_BUILD_OPENGL})
    add_dependencies(HyperEngine HyperOpenGL33)
    add_dependencies(HyperEngine HyperOpenGL46)
endif ()

#-------------------------------------------------------------------------------------------
# Vulkan
#-------------------------------------------------------------------------------------------
if (${HYPERENGINE_BUILD_VULKAN})
    add_dependencies(HyperEngine HyperVulkan)
endif ()
