#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperPlatform
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperPlatform/Window.cpp)

set(HEADERS
        include/HyperPlatform/PlatformDetection.hpp
        include/HyperPlatform/Window.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperPlatform)
target_link_libraries(
        HyperPlatform
        PUBLIC
        CONAN_PKG::glfw
        HyperCore)
