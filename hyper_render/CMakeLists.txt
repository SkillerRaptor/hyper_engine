#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_render/renderer.cpp)

set(HEADERS
        include/hyper_render/renderer.hpp)

hyperengine_define_library(hyper_render)
target_link_libraries(
        hyper_render
        PUBLIC
        hyper_core
        hyper_rhi
        glm
        meshoptimizer)

