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
# HyperResource
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperResource/ShaderCompiler.cpp)

set(HEADERS
        include/HyperResource/ShaderCompiler.hpp)

hyperengine_group_source()

hyperengine_define_module(HyperResource)
target_link_libraries(HyperResource PUBLIC HyperCore)
target_link_libraries(HyperResource PUBLIC assimp)
target_link_libraries(HyperResource PUBLIC glslang)
target_link_libraries(HyperResource PUBLIC meshoptimizer)
target_link_libraries(HyperResource PUBLIC spirv-cross-core spirv-cross-hlsl spirv-cross-glsl spirv-cross-cpp spirv-reflect)
target_link_libraries(HyperResource PUBLIC SPIRV-Tools)
target_link_libraries(HyperResource PUBLIC stb_image)
