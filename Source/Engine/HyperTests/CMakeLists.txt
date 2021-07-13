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
        src/HyperTests/HyperCore/SparsePoolAllocatorTest.cpp
        src/HyperTests/HyperMath/Vector2.cpp
        src/HyperTests/HyperMath/Vector3.cpp
        src/HyperTests/HyperMath/Vector4.cpp)

set(HEADERS
        "")

hyperengine_group_source()

hyperengine_define_executable(HyperTests)
target_link_libraries(HyperTests PUBLIC HyperCore)
target_link_libraries(HyperTests PUBLIC HyperMath)
target_link_libraries(HyperTests PUBLIC Catch2)
