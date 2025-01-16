#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_windowing/key_events.cpp
        src/hyper_windowing/mouse_events.cpp
        src/hyper_windowing/window.cpp
        src/hyper_windowing/window_events.cpp)

set(HEADERS
        include/hyper_windowing/event.hpp
        include/hyper_windowing/forward.hpp
        include/hyper_windowing/key_codes.hpp
        include/hyper_windowing/key_events.hpp
        include/hyper_windowing/mouse_codes.hpp
        include/hyper_windowing/mouse_events.hpp
        include/hyper_windowing/window.hpp
        include/hyper_windowing/window_events.hpp)

hyperengine_define_library(hyper_windowing)
target_link_libraries(
        hyper_windowing
        PUBLIC
        hyper_core
        glm
        SDL3::SDL3)

