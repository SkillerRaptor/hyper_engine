#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
    src/hyper_core/logger.cpp)

set(HEADERS
    include/hyper_core/logger.hpp)

hyperengine_define_library(hyper_core)
target_link_libraries(
    hyper_core
    PUBLIC
    fmt)
