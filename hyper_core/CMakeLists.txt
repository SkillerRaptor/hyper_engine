#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_core/filesystem.cpp
        src/hyper_core/job_system.cpp
        src/hyper_core/logger.cpp
        src/hyper_core/string.cpp)

set(HEADERS
        include/hyper_core/assertion.hpp
        include/hyper_core/bit_flags.hpp
        include/hyper_core/bits.hpp
        include/hyper_core/filesystem.hpp
        include/hyper_core/forward.hpp
        include/hyper_core/job_system.hpp
        include/hyper_core/logger.hpp
        include/hyper_core/prerequisites.hpp
        include/hyper_core/string.hpp
        include/hyper_core/thread_safe_ring_buffer.hpp)

hyperengine_define_library(hyper_core)
target_link_libraries(
        hyper_core
        PUBLIC
        fmt
        glm
        libassert::assert
        spdlog)
target_compile_definitions(
        hyper_core
        PUBLIC
        -DGLM_FORCE_RADIANS
        -DGLM_FORCE_DEPTH_ZERO_TO_ONE)