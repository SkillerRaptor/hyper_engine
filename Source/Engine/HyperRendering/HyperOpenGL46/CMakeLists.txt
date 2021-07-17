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
# HyperOpenGL46
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperRendering/OpenGL46/Context.cpp)

set(HEADERS
        include/HyperRendering/OpenGL46/Context.hpp)

hyperengine_group_source()

hyperengine_define_module(HyperOpenGL46)
target_include_directories(HyperOpenGL46 PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(HyperOpenGL46 PUBLIC HyperCore)
target_link_libraries(HyperOpenGL46 PUBLIC HyperPlatform)
target_link_libraries(HyperOpenGL46 PUBLIC HyperRenderingBase)
target_link_libraries(HyperOpenGL46 PRIVATE ${OPENGL_LIBRARIES})
