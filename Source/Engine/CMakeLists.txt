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
        src/HyperEngine/Error.cpp)

set(HEADERS
        include/HyperEngine/Application.hpp
        include/HyperEngine/EngineLoop.hpp
        include/HyperEngine/Error.hpp
        include/HyperEngine/Expected.hpp
        include/HyperEngine/Launcher.hpp
        include/HyperEngine/Prerequisites.hpp)

add_library(HyperEngine STATIC ${SOURCES} ${HEADERS})
target_link_libraries(
        HyperEngine
        PRIVATE
        ProjectOptions
        ProjectWarnings)
target_include_directories(
        HyperEngine
        PUBLIC
        include)
target_link_libraries(
        HyperEngine
        PUBLIC
        glfw
        Vulkan-Headers
        volk)
