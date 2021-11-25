#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEditor
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/main.cpp
        src/HyperEditor/Editor.cpp)

set(HEADERS
        include/HyperEditor/Editor.hpp)


add_executable(HyperEditor ${SOURCES} ${HEADERS})
target_link_libraries(
        HyperEditor
        PRIVATE
        ProjectOptions
        ProjectWarnings
        HyperEngine)
target_include_directories(
        HyperEditor
        PUBLIC
        include)
set_property(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY
        VS_STARTUP_PROJECT "HyperEditor")
