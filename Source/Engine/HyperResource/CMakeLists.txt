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
# HyperResource
#-------------------------------------------------------------------------------------------
set(HYPERENGINE_DXC_PATH ${CMAKE_SOURCE_DIR}/Source/ThirdParty/DirectXShaderCompiler)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperResource STATIC ${SOURCES} ${HEADERS})
set_target_properties(HyperResource PROPERTIES FOLDER HyperModules)
target_include_directories(HyperResource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(HyperResource PUBLIC ${HYPERENGINE_DXC_PATH}/include)
target_link_libraries(HyperResource PUBLIC HyperCore)
target_link_libraries(HyperResource PUBLIC HyperPlatform)
target_link_libraries(HyperResource PUBLIC dxcompiler)
target_link_libraries(HyperResource PUBLIC LLVMDxcSupport)
target_link_libraries(HyperResource PUBLIC LLVMSupport)
target_link_libraries(HyperResource PUBLIC spirv-cross-core)
target_link_libraries(HyperResource PUBLIC spirv-cross-hlsl)
target_link_libraries(HyperResource PUBLIC spirv-cross-glsl)
target_link_libraries(HyperResource PUBLIC spirv-cross-cpp)
target_link_libraries(HyperResource PUBLIC spirv-reflect)
