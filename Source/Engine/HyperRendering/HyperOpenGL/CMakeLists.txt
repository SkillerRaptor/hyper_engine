#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperOpenGL
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperOpenGL/GraphicsContext.cpp)

set(HEADERS
        include/HyperOpenGL/GraphicsContext.hpp)

hyperengine_define_module(HyperOpenGL)
target_link_libraries(
        HyperOpenGL
        PUBLIC
        CONAN_PKG::glad
        CONAN_PKG::glfw
        HyperRenderingBase)
