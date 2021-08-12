#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperEngine/Application.cpp
        src/HyperEngine/EngineLoop.cpp
        src/HyperEngine/Launcher.cpp)

set(HEADERS
        include/HyperEngine/Application.hpp
        include/HyperEngine/EngineLoop.hpp
        include/HyperEngine/Launcher.hpp)

hyperengine_group_source(${SOURCES})
hyperengine_group_source(${HEADERS})

hyperengine_define_library(HyperEngine)
target_link_libraries(
        HyperEngine
        PUBLIC
        HyperCore
        HyperMath
        HyperPlatform)
