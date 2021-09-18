#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperGame
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperGame/GameModule.cpp
        src/HyperGame/EntitySystem/Registry.cpp
        src/HyperGame/EventSystem/EventManager.cpp)

set(HEADERS
        include/HyperGame/GameModule.hpp
        include/HyperGame/MouseButton.hpp
        include/HyperGame/EntitySystem/ComponentPool.hpp
        include/HyperGame/EntitySystem/Components.hpp
        include/HyperGame/EntitySystem/ComponentType.hpp
        include/HyperGame/EntitySystem/Entity.hpp
        include/HyperGame/EntitySystem/Registry.hpp
        include/HyperGame/EntitySystem/Components/TagComponent.hpp
        include/HyperGame/EventSystem/EventCallback.hpp
        include/HyperGame/EventSystem/EventManager.hpp
        include/HyperGame/EventSystem/EventType.hpp
        include/HyperGame/EventSystem/Listener.hpp
        include/HyperGame/EventSystem/Events/KeyEvents.hpp
        include/HyperGame/EventSystem/Events/MouseEvents.hpp
        include/HyperGame/EventSystem/Events/WindowEvents.hpp)

hyperengine_create_module(HyperGame)
target_link_libraries(
        HyperGame
        PUBLIC
        HyperCore
        HyperMath)
