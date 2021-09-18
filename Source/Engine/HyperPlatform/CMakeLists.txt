#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperPlatform
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperPlatform/PlatformModule.cpp
        src/HyperPlatform/Window.cpp)

set(HEADERS
        include/HyperPlatform/RendererType.hpp
        include/HyperPlatform/PlatformDetection.hpp
        include/HyperPlatform/PlatformModule.hpp
        include/HyperPlatform/Window.hpp)

hyperengine_create_module(HyperPlatform)
target_link_libraries(
        HyperPlatform
        PUBLIC
        CONAN_PKG::glfw
        HyperCore)
