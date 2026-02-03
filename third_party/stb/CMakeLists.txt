#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

add_library(
        stb
        src/stb_image.cpp
        ${stb_SOURCE_DIR}/stb_image.h)
target_include_directories(
        stb
        SYSTEM
        PUBLIC
        ${stb_SOURCE_DIR})
