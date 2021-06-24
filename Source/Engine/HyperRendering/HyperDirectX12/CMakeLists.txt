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
# HyperDirectX12
#-------------------------------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/Source/Engine/HyperRendering/HyperCommon/HyperCommon.cmake)

set(CORE_SOURCES
        src/HyperRendering/DirectX12/DirectX12.cpp)

set(CORE_HEADERS
        "")

set(SOURCES ${CORE_SOURCES} ${HYPERCOMMON_SOURCES})
set(HEADERS ${CORE_HEADERS} ${HYPERCOMMON_HEADERS})

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperDirectX12 SHARED ${SOURCES} ${HEADERS})
set_target_properties(HyperDirectX12 PROPERTIES FOLDER HyperModules/HyperRendering)
target_include_directories(HyperDirectX12 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${HYPERCOMMON_INCLUDE})
target_link_libraries(HyperDirectX12 PRIVATE HyperCore HyperMath HyperPlatform)

add_custom_command(
        TARGET HyperDirectX12
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:HyperDirectX12> ${CMAKE_BINARY_DIR}/Source/Editor/)
