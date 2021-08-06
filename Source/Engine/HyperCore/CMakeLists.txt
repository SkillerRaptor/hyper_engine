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
        src/HyperCore/Logger.cpp
        src/HyperCore/Events/EventManager.cpp
        src/HyperCore/Events/Event.cpp)

set(HEADERS
        include/HyperCore/Assertion.hpp
        include/HyperCore/Bits.hpp
        include/HyperCore/Compilers.hpp
        include/HyperCore/Formatter.hpp
        include/HyperCore/Logger.hpp
        include/HyperCore/Prerequisites.hpp
        include/HyperCore/Result.hpp
        include/HyperCore/SourceLocation.hpp
        include/HyperCore/Compilers/ClangSpecific.hpp
        include/HyperCore/Compilers/GCCSpecific.hpp
        include/HyperCore/Compilers/MSVCSpecific.hpp
        include/HyperCore/Events/Event.hpp
        include/HyperCore/Events/EventManager.hpp
        include/HyperCore/Events/EventWrapper.hpp
        include/HyperCore/Events/IEvent.hpp
        include/HyperCore/Events/KeyEvents.hpp
        include/HyperCore/Events/MouseEvents.hpp
        include/HyperCore/Events/WindowEvents.hpp
        include/HyperCore/Memory/RingBuffer.hpp
        include/HyperCore/Memory/SparsePool.hpp)

group_source()

define_module(HyperCore)
target_link_libraries(HyperCore PUBLIC CONAN_PKG::fmt)

if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_DEBUG=1)
else ()
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_RELEASE=1)
endif ()
