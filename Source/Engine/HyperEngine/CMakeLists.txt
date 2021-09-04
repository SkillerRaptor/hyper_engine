#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperEngine/EngineLoop.cpp
        src/HyperEngine/IApplication.cpp
        src/HyperEngine/Launcher.cpp)

set(HEADERS
        include/HyperEngine/EngineLoop.hpp
        include/HyperEngine/IApplication.hpp
        include/HyperEngine/Launcher.hpp)

hyperengine_define_library(HyperEngine)
target_link_libraries(
        HyperEngine
        PUBLIC
        HyperCore
        HyperGame
        HyperMath
        HyperPlatform
        HyperRendering
        HyperResource)
