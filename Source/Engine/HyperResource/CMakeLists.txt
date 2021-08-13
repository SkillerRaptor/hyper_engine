#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperResource
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperResource/ShaderCompiler.cpp)

set(HEADERS
        include/HyperResource/ShaderCompiler.hpp)

hyperengine_group_source(${SOURCES})
hyperengine_group_source(${HEADERS})

hyperengine_define_module(HyperResource)
target_link_libraries(
        HyperResource
        PUBLIC
        CONAN_PKG::assimp
        CONAN_PKG::glslang
        CONAN_PKG::meshoptimizer
        CONAN_PKG::spirv-cross
        CONAN_PKG::stb
        HyperCore)
