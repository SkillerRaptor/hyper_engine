#-------------------------------------------------------------------------------------------
# Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# HyperPlatform
#-------------------------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperPlatform STATIC ${SOURCES} ${HEADERS})
set_target_properties(HyperPlatform PROPERTIES FOLDER HyperModules)
set_target_properties(HyperPlatform PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(HyperPlatform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(HyperPlatform PUBLIC ${CMAKE_DL_LIBS} HyperCore)

if ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    find_package(X11)
    target_include_directories(HyperPlatform PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(HyperPlatform PUBLIC ${X11_LIBRARIES})
endif()

#-------------------------------------------------------------------------------------------
# Graphic APIs
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# DirectX
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    target_compile_definitions(HyperPlatform PUBLIC HYPERENGINE_BUILD_DIRECTX11=1)
    target_compile_definitions(HyperPlatform PUBLIC HYPERENGINE_BUILD_DIRECTX12=1)
endif ()

#-------------------------------------------------------------------------------------------
# OpenGL
#-------------------------------------------------------------------------------------------
find_package(OpenGL)
if (OPENGL_FOUND)
    target_compile_definitions(HyperPlatform PUBLIC HYPERENGINE_BUILD_OPENGL33=1)
    target_compile_definitions(HyperPlatform PUBLIC HYPERENGINE_BUILD_OPENGL46=1)
endif ()

#-------------------------------------------------------------------------------------------
# Vulkan
#-------------------------------------------------------------------------------------------
find_package(Vulkan)
if (Vulkan_FOUND)
    target_compile_definitions(HyperPlatform PUBLIC HYPERENGINE_BUILD_VULKAN=1)
endif ()
