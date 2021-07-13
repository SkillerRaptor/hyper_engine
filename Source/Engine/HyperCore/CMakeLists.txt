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
# HyperCore
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperCore/Formatter.cpp
        src/HyperCore/Logger.cpp
        src/HyperCore/Events/EventManager.cpp
        src/HyperCore/Events/Event.cpp)

set(HEADERS
        include/HyperCore/Assertion.hpp
        include/HyperCore/Formatter.hpp
        include/HyperCore/Logger.hpp
        include/HyperCore/Containers/SparseSet.hpp
        include/HyperCore/Events/Event.hpp
        include/HyperCore/Events/EventManager.hpp
        include/HyperCore/Events/EventWrapper.hpp
        include/HyperCore/Events/IEvent.hpp
        include/HyperCore/Events/KeyEvents.hpp
        include/HyperCore/Events/MouseEvents.hpp
        include/HyperCore/Events/WindowEvents.hpp
        include/HyperCore/Memory/SparsePoolAllocator.hpp
        include/HyperCore/Utilities/Bits.hpp
        include/HyperCore/Utilities/Macros.hpp
        include/HyperCore/Utilities/Result.hpp
        include/HyperCore/Utilities/SourceLocation.hpp)

hyperengine_group_source()

hyperengine_define_module(HyperCore)
target_link_libraries(HyperCore PUBLIC fmt)

if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_DEBUG=1)
else ()
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_RELEASE=1)
endif ()

if ("${HYPERENGINE_COMPILER}" STREQUAL "Clang")
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_COMPILER_Clang=1)
elseif ("${HYPERENGINE_COMPILER}" STREQUAL "GNU")
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_COMPILER_GNU=1)
elseif ("${HYPERENGINE_COMPILER}" STREQUAL "MSVC")
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_COMPILER_MSVC=1)
endif ()
