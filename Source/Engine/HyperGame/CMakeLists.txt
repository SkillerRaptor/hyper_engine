#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperGame
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperGame/Events/EventManager.cpp)

set(HEADERS
        include/HyperGame/Events/EventCallback.hpp
        include/HyperGame/Events/EventManager.hpp
        include/HyperGame/Events/EventType.hpp
        include/HyperGame/Events/KeyEvents.hpp
        include/HyperGame/Events/Listener.hpp
        include/HyperGame/Events/MouseEvents.hpp
        include/HyperGame/Events/WindowEvents.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperGame)
target_link_libraries(
        HyperGame
        PUBLIC
        HyperCore
        HyperMath)
