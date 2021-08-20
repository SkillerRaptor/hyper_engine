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

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_library(HyperEngine)
target_link_libraries(
        HyperEngine
        PUBLIC
        HyperCore
        HyperMath
        HyperPlatform
        HyperResource)

#-------------------------------------------------------------------------------------------
# OpenGL
#-------------------------------------------------------------------------------------------
if (HYPERENGINE_BUILD_OPENGL)
    # TODO(SkillerRaptor): Implementing OpenGL 4.6
    target_link_libraries(
            HyperEngine
            PUBLIC
            HyperOpenGL33)
    target_compile_definitions(
            HyperEngine
            PUBLIC
            -DHYPERENGINE_BUILD_OPENGL=1)
endif ()

#-------------------------------------------------------------------------------------------
# Vulkan
#-------------------------------------------------------------------------------------------
if (HYPERENGINE_BUILD_VULKAN)
    target_link_libraries(
            HyperEngine
            PUBLIC
            HyperVulkan)
    target_compile_definitions(
            HyperEngine
            PUBLIC
            -DHYPERENGINE_BUILD_VULKAN=1)
endif ()
