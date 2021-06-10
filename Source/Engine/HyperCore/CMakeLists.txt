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
# HyperCore
#-------------------------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperCore STATIC ${SOURCES} ${HEADERS})
set_target_properties(HyperCore PROPERTIES FOLDER HyperModules)
target_include_directories(HyperCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(HyperCore PUBLIC fmt)

if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_DEBUG=1)
else ()
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_RELEASE=1)
endif ()
