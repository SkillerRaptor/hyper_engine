#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperVulkan
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperVulkan/Context.cpp)

set(HEADERS
        include/HyperVulkan/Context.hpp)

hyperengine_group_source("${SOURCES}")
hyperengine_group_source("${HEADERS}")

hyperengine_define_module(HyperVulkan)
target_link_libraries(
        HyperVulkan
        PUBLIC
        CONAN_PKG::glfw
        CONAN_PKG::volk
        HyperRendering)
