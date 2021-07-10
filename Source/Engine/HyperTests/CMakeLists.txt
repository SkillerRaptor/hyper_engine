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
# HyperTests
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperTests/Placeholder.cpp)

set(HEADERS
        "")

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperTests STATIC ${SOURCES} ${HEADERS})
set_target_properties(HyperTests PROPERTIES FOLDER HyperModules)
target_include_directories(HyperTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(HyperTests PUBLIC HyperCore)
target_link_libraries(HyperTests PUBLIC Catch2)
