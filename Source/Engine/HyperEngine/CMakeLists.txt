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
        src/HyperEngine/Launcher.cpp)

set(HEADERS
        include/HyperEngine/EngineLoop.hpp
        include/HyperEngine/IApplication.hpp
        include/HyperEngine/Launcher.hpp)

group_source()

define_library(HyperEngine)
target_link_libraries(HyperEngine PUBLIC HyperCore)
target_link_libraries(HyperEngine PUBLIC HyperMath)
target_link_libraries(HyperEngine PUBLIC HyperPlatform)
target_link_libraries(HyperEngine PUBLIC HyperResource)

#-------------------------------------------------------------------------------------------
# DirectX
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    target_link_libraries(HyperEngine PUBLIC HyperDirectX11)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_DIRECTX11=1)

    target_link_libraries(HyperEngine PUBLIC HyperDirectX12)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_DIRECTX12=1)
endif ()

#-------------------------------------------------------------------------------------------
# OpenGL
#-------------------------------------------------------------------------------------------
if (${HYPERENGINE_BUILD_OPENGL})
    target_link_libraries(HyperEngine PUBLIC HyperOpenGL33)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_OPENGL33=1)

    target_link_libraries(HyperEngine PUBLIC HyperOpenGL46)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_OPENGL46=1)
endif ()

#-------------------------------------------------------------------------------------------
# Vulkan
#-------------------------------------------------------------------------------------------
if (${HYPERENGINE_BUILD_VULKAN})
    # target_link_libraries(HyperEngine PUBLIC HyperVulkan)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_VULKAN=1)
endif ()
