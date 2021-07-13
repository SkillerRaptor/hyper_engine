#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# HyperOpenGL33
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(PLATFORM_SOURCES
            src/HyperRendering/OpenGL33/Windows/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            include/HyperRendering/OpenGL33/Windows/PlatformContext.hpp
            include/HyperRendering/OpenGL33/Windows/PlatformInclude.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(PLATFORM_SOURCES
            src/HyperRendering/OpenGL33/Linux/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            include/HyperRendering/OpenGL33/Linux/PlatformContext.hpp
            include/HyperRendering/OpenGL33/Linux/PlatformInclude.hpp)
endif ()

set(CORE_SOURCES
        src/HyperRendering/OpenGL33/Context.cpp
        src/HyperRendering/OpenGL33/IPlatformContext.cpp
        src/HyperRendering/OpenGL33/Renderer.cpp)

set(CORE_HEADERS
        include/HyperRendering/OpenGL33/Context.hpp
        include/HyperRendering/OpenGL33/IPlatformContext.hpp
        include/HyperRendering/OpenGL33/PlatformInclude.hpp
        include/HyperRendering/OpenGL33/Renderer.hpp)

set(SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES} ${HYPERCOMMON_SOURCES})
set(HEADERS ${CORE_HEADERS} ${PLATFORM_HEADERS} ${HYPERCOMMON_HEADERS})

hyperengine_group_source()

hyperengine_define_module(HyperOpenGL33)
target_include_directories(HyperOpenGL33 PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(HyperOpenGL33 PUBLIC HyperCore)
target_link_libraries(HyperOpenGL33 PUBLIC HyperPlatform)
target_link_libraries(HyperOpenGL33 PUBLIC HyperRenderingBase)
target_link_libraries(HyperOpenGL33 PRIVATE ${OPENGL_LIBRARIES})
