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

hyperengine_group_source(${SOURCES})
hyperengine_group_source(${HEADERS})

hyperengine_define_library(HyperEngine)
target_link_libraries(
        HyperEngine
        PUBLIC
        HyperCore
        HyperMath
        HyperPlatform
        HyperResource)

#-------------------------------------------------------------------------------------------
# DirectX
#-------------------------------------------------------------------------------------------
if (HYPERENGINE_BUILD_DIRECTX)
    # TODO: Implement DirectX11 & DirectX12
endif ()

#-------------------------------------------------------------------------------------------
# OpenGL
#-------------------------------------------------------------------------------------------
if (HYPERENGINE_BUILD_OPENGL)
    # TODO: Implement OpenGL 4.6
    target_link_libraries(
            HyperEngine
            PUBLIC
            HyperOpenGL33)
endif ()

#-------------------------------------------------------------------------------------------
# Vulkan
#-------------------------------------------------------------------------------------------
if (HYPERENGINE_BUILD_VULKAN)
    # TODO: Implement Vulkan
endif ()
