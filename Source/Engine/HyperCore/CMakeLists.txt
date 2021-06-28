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
# HyperCore
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperCore/Logger.cpp
        src/HyperCore/Events/EventManager.cpp
        src/HyperCore/Events/Event.cpp)

set(HEADERS
        src/HyperCore/Assertion.hpp
        src/HyperCore/Formatter.hpp
        src/HyperCore/Logger.hpp
        src/HyperCore/Events/Event.hpp
        src/HyperCore/Events/EventManager.hpp
        src/HyperCore/Events/EventWrapper.hpp
        src/HyperCore/Events/IEvent.hpp
        src/HyperCore/Events/KeyEvents.hpp
        src/HyperCore/Events/MouseEvents.hpp
        src/HyperCore/Events/WindowEvents.hpp
        src/HyperCore/Memory/SparsePoolAllocator.hpp
        src/HyperCore/Utilities/Prerequisites.hpp
        src/HyperCore/Utilities/SourceLocation.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperCore STATIC ${SOURCES} ${HEADERS})
set_target_properties(HyperCore PROPERTIES FOLDER HyperModules)
set_target_properties(HyperCore PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(HyperCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(HyperCore PUBLIC fmt)

if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_DEBUG=1)
else ()
    target_compile_definitions(HyperCore PUBLIC HYPERENGINE_RELEASE=1)
endif ()
