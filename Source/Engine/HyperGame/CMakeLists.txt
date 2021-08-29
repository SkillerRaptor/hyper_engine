#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperGame
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperGame/EntitySystem/Registry.cpp
        src/HyperGame/Events/EventManager.cpp)

set(HEADERS
        include/HyperGame/MouseButton.hpp
        include/HyperGame/EntitySystem/ComponentPool.hpp
        include/HyperGame/EntitySystem/Components.hpp
        include/HyperGame/EntitySystem/ComponentType.hpp
        include/HyperGame/EntitySystem/Entity.hpp
        include/HyperGame/EntitySystem/Registry.hpp
        include/HyperGame/EntitySystem/Components/TagComponent.hpp
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
