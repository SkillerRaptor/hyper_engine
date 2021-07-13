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
# HyperTests
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperTests/main.cpp
        src/HyperTests/HyperCore/SparsePoolAllocatorTest.cpp)

set(HEADERS
        "")

hyperengine_group_source()

hyperengine_define_module(HyperTests)
target_link_libraries(HyperTests PUBLIC HyperCore)
target_link_libraries(HyperTests PUBLIC Catch2)
