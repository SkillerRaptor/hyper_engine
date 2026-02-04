#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

set(SOURCES
        src/hyper_core/filesystem.cpp
        src/hyper_core/logger.cpp
        src/hyper_core/string_utils.cpp)

set(HEADERS
        include/hyper_core/assertion.hpp
        include/hyper_core/bit_flags.hpp
        include/hyper_core/filesystem.hpp
        include/hyper_core/logger.hpp
        include/hyper_core/memory.hpp
        include/hyper_core/prerequisites.hpp
        include/hyper_core/string_utils.hpp
        include/hyper_core/types.hpp)

he_add_library(hyper_core)
target_compile_definitions(
        hyper_core
        PRIVATE
        SPDLOG_NO_SOURCE_LOC)
target_link_libraries(
        hyper_core
        PUBLIC
        fmt::fmt)
target_link_libraries(
        hyper_core
        PRIVATE
        spdlog::spdlog)