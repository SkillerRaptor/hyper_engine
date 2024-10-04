#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_event/event_bus.cpp)

set(HEADERS
        include/hyper_event/event_bus.hpp)

hyperengine_define_library(hyper_event)
target_link_libraries(
        hyper_event
        PUBLIC
        hyper_core)
