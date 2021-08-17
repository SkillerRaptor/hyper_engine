#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperOpenGL33
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperOpenGL33/Context.cpp)

set(HEADERS
        include/HyperOpenGL33/Context.hpp)

hyperengine_group_source("${SOURCES}")
hyperengine_group_source("${HEADERS}")

hyperengine_define_module(HyperOpenGL33)
target_link_libraries(
        HyperOpenGL33
        PUBLIC
        CONAN_PKG::glad
        CONAN_PKG::glfw
        HyperRendering)
