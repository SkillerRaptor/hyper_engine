#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperVulkan
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperVulkan/Context.cpp
        src/HyperVulkan/Device.cpp)

set(HEADERS
        include/HyperVulkan/Context.hpp
        include/HyperVulkan/Device.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperVulkan)
target_link_libraries(
        HyperVulkan
        PUBLIC
        CONAN_PKG::glfw
        CONAN_PKG::volk
        HyperRendering)
