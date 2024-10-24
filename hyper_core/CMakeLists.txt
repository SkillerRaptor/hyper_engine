#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_core/filesystem.cpp
        src/hyper_core/logger.cpp
        src/hyper_core/string.cpp)

set(HEADERS
        include/hyper_core/assertion.hpp
        include/hyper_core/filesystem.hpp
        include/hyper_core/logger.hpp
        include/hyper_core/prerequisites.hpp
        include/hyper_core/string.hpp)

hyperengine_define_library(hyper_core)
target_link_libraries(
        hyper_core
        PUBLIC
        fmt
        libassert::assert)
