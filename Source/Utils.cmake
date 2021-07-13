#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

function(hyperengine_group_source)
    foreach (item IN ITEMS ${SOURCES} ${HEADERS})
        get_filename_component(src_path "${item}" PATH)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
        string(REPLACE ".." "\\" group_path "${group_path}")
        source_group("${group_path}" FILES "${item}")
    endforeach ()
endfunction()

function(hyperengine_define_executable target)
    add_executable(${target} ${SOURCES} ${HEADERS})
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_compile_options(${target} PRIVATE ${HYPERENGINE_WARNING_FLAGS})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endfunction()

function(hyperengine_define_library target)
    add_library(${target} STATIC ${SOURCES} ${HEADERS})
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_compile_options(${target} PRIVATE ${HYPERENGINE_WARNING_FLAGS})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endfunction()

function(hyperengine_define_module target)
    hyperengine_define_library(${target})
    set_target_properties(${target} PROPERTIES FOLDER HyperModules)
endfunction()
