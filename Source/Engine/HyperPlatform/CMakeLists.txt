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
# HyperPlatform
#-------------------------------------------------------------------------------------------
set(CORE_SOURCES
        src/HyperPlatform/LibraryManager.cpp)

set(CORE_HEADERS
        include/HyperPlatform/FileSystem.hpp
        include/HyperPlatform/LibraryManager.hpp
        include/HyperPlatform/PlatformDetection.hpp
        include/HyperPlatform/SharedLibrary.hpp
        include/HyperPlatform/Window.hpp)

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(PLATFORM_SOURCES
            src/HyperPlatform/Windows/FileSystem.cpp
            src/HyperPlatform/Windows/LibraryManager.cpp
            src/HyperPlatform/Windows/Window.cpp)

    set(PLATFORM_HEADERS
            include/HyperPlatform/Windows/SharedLibrary.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(PLATFORM_SOURCES
            src/HyperPlatform/Linux/FileSystem.cpp
            src/HyperPlatform/Linux/LibraryManager.cpp
            src/HyperPlatform/Linux/Window.cpp)

    set(PLATFORM_HEADERS
            include/HyperPlatform/Linux/SharedLibrary.hpp)
endif ()

set(SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES})
set(HEADERS ${CORE_HEADERS} ${PLATFORM_HEADERS})

group_source()

define_module(HyperPlatform)
target_link_libraries(HyperPlatform PUBLIC ${CMAKE_DL_LIBS} HyperCore)

if ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    find_package(X11)
    target_include_directories(HyperPlatform PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(HyperPlatform PUBLIC ${X11_LIBRARIES})
endif ()
