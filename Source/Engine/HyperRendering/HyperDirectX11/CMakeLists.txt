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
# HyperDirectX11
#-------------------------------------------------------------------------------------------
set(HYPERENGINE_COMMON_PATH ${CMAKE_SOURCE_DIR}/Source/Engine/HyperRendering/HyperCommon)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${HYPERENGINE_COMMON_PATH}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp ${HYPERENGINE_COMMON_PATH}/src/*.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperDirectX11 SHARED ${SOURCES} ${HEADERS})
set_target_properties(HyperDirectX11 PROPERTIES FOLDER HyperModules/HyperRendering)
target_include_directories(HyperDirectX11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${HYPERENGINE_COMMON_PATH}/src)
target_link_libraries(HyperDirectX11 PRIVATE HyperCore HyperMath HyperPlatform)

add_custom_command(
        TARGET HyperDirectX11
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:HyperDirectX11> ${CMAKE_BINARY_DIR}/Source/Editor/)
