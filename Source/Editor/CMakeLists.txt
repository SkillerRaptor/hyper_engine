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
# HyperEditor
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/main.cpp
        src/HyperEditor/Editor.cpp)

set(HEADERS
        src/HyperEditor/Editor.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_executable(HyperEditor ${SOURCES} ${HEADERS})
target_include_directories(HyperEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(HyperEditor PRIVATE HyperEngine)

#add_custom_command(
#        TARGET HyperEditor
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
