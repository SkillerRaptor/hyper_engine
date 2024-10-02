#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
    src/hyper_platform/window.cpp)

set(HEADERS
    include/hyper_platform/window.hpp)

hyperengine_define_library(hyper_platform)
target_link_libraries(
    hyper_platform
    PUBLIC
    hyper_core
    glfw)
