#-------------------------------------------------------------------------------------------
# Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
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
include(${CMAKE_SOURCE_DIR}/Source/Engine/HyperRendering/HyperCommon/HyperCommon.cmake)

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(PLATFORM_SOURCES
            src/HyperRendering/OpenGL33/Windows/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            src/HyperRendering/OpenGL33/Windows/PlatformContext.hpp
            src/HyperRendering/OpenGL33/Windows/PlatformInclude.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(PLATFORM_SOURCES
            src/HyperRendering/OpenGL33/Linux/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            src/HyperRendering/OpenGL33/Linux/PlatformContext.hpp
            src/HyperRendering/OpenGL33/Linux/PlatformInclude.hpp)
endif ()

set(CORE_SOURCES
        src/HyperRendering/OpenGL33/Context.cpp
        src/HyperRendering/OpenGL33/IPlatformContext.cpp
        src/HyperRendering/OpenGL33/Renderer.cpp)

set(CORE_HEADERS
        src/HyperRendering/OpenGL33/Context.hpp
        src/HyperRendering/OpenGL33/IPlatformContext.hpp
        src/HyperRendering/OpenGL33/PlatformInclude.hpp
        src/HyperRendering/OpenGL33/Renderer.hpp)

set(SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES} ${HYPERCOMMON_SOURCES})
set(HEADERS ${CORE_HEADERS} ${PLATFORM_HEADERS} ${HYPERCOMMON_HEADERS})

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperOpenGL33 SHARED ${SOURCES} ${HEADERS})
set_target_properties(HyperOpenGL33 PROPERTIES FOLDER HyperModules/HyperRendering)
target_include_directories(HyperOpenGL33 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${HYPERCOMMON_INCLUDE})
target_link_libraries(HyperOpenGL33 PRIVATE HyperCore HyperMath HyperPlatform)

find_package(OpenGL)
target_include_directories(HyperOpenGL33 PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(HyperOpenGL33 PRIVATE ${OPENGL_LIBRARIES})

add_custom_command(
        TARGET HyperOpenGL33
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:HyperOpenGL33> ${CMAKE_BINARY_DIR}/Source/Editor/)
