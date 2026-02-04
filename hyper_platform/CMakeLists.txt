#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

set(SOURCES
        src/hyper_platform/input.cpp
        src/hyper_platform/window.cpp)

set(HEADERS
        include/hyper_platform/input.hpp
        include/hyper_platform/key_codes.hpp
        include/hyper_platform/mouse_codes.hpp
        include/hyper_platform/window.hpp)

he_add_library(hyper_platform)
target_link_libraries(
        hyper_platform
        PRIVATE
        hyper_core
        SDL3::SDL3)