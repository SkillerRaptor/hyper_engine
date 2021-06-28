#-------------------------------------------------------------------------------------------
# Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# stb_image
#-------------------------------------------------------------------------------------------
project(stb_image LANGUAGES C)

set(SOURCES
        src/stb_image.c)

set(HEADERS
        include/stb_image.h)

add_library(stb_image STATIC ${SOURCES} ${HEADERS})
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE C)
