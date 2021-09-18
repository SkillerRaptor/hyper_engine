#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperResource
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperResource/ResourceModule.cpp
        src/HyperResource/ShaderCompiler.cpp)

set(HEADERS
        include/HyperResource/ResourceModule.hpp
        include/HyperResource/ShaderCompiler.hpp)

hyperengine_create_module(HyperResource)
target_link_libraries(
        HyperResource
        PUBLIC
        CONAN_PKG::glslang
        HyperCore)
