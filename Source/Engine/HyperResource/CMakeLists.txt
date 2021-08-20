#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperResource
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperResource/HyperResource.cpp)

set(HEADERS
        )

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperResource)
target_link_libraries(
        HyperResource
        PUBLIC
        CONAN_PKG::glslang
        CONAN_PKG::meshoptimizer
        CONAN_PKG::spirv-cross
        CONAN_PKG::stb
        CONAN_PKG::tinygltf
        CONAN_PKG::tinyobjloader
        HyperCore)
