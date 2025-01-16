#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/main.cpp
        src/hyper_engine/camera.cpp
        src/hyper_engine/editor_engine.cpp
        src/hyper_engine/engine.cpp
        src/hyper_engine/engine_loop.cpp
        src/hyper_engine/game_engine.cpp
        src/hyper_engine/input.cpp)

set(HEADERS
        include/hyper_engine/camera.hpp
        include/hyper_engine/editor_engine.hpp
        include/hyper_engine/engine.hpp
        include/hyper_engine/engine_loop.hpp
        include/hyper_engine/game_engine.hpp
        include/hyper_engine/input.hpp)

hyperengine_define_executable(hyper_engine)
target_link_libraries(
        hyper_engine
        PRIVATE
        hyper_core
        hyper_windowing
        hyper_rhi
        hyper_rendering
        argparse)

