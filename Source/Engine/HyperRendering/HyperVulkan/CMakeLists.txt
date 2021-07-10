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
# HyperVulkan
#-------------------------------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/Source/Engine/HyperRendering/HyperCommon/HyperCommon.cmake)

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(PLATFORM_SOURCES
            src/HyperRendering/Vulkan/Windows/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            src/HyperRendering/Vulkan/Windows/PlatformContext.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(PLATFORM_SOURCES
            src/HyperRendering/Vulkan/Linux/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            src/HyperRendering/Vulkan/Linux/PlatformContext.hpp)
endif ()

set(CORE_SOURCES
        src/HyperRendering/Vulkan/Context.cpp
        src/HyperRendering/Vulkan/Device.cpp
        src/HyperRendering/Vulkan/IPlatformContext.cpp
        src/HyperRendering/Vulkan/Renderer.cpp
        src/HyperRendering/Vulkan/SwapChain.cpp)

set(CORE_HEADERS
        src/HyperRendering/Vulkan/Context.hpp
        src/HyperRendering/Vulkan/Device.hpp
        src/HyperRendering/Vulkan/IPlatformContext.hpp
        src/HyperRendering/Vulkan/Renderer.hpp
        src/HyperRendering/Vulkan/SwapChain.hpp)

set(SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES} ${HYPERCOMMON_SOURCES})
set(HEADERS ${CORE_HEADERS} ${PLATFORM_HEADERS} ${HYPERCOMMON_HEADERS})

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperVulkan SHARED ${SOURCES} ${HEADERS})
set_target_properties(HyperVulkan PROPERTIES FOLDER HyperModules/HyperRendering)
target_include_directories(HyperVulkan PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${HYPERCOMMON_INCLUDE})
target_link_libraries(HyperVulkan PRIVATE HyperCore HyperMath HyperPlatform)

find_package(Vulkan)
target_include_directories(HyperVulkan PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(HyperVulkan PRIVATE ${Vulkan_LIBRARIES})

add_custom_command(
        TARGET HyperVulkan
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:HyperVulkan> ${CMAKE_BINARY_DIR}/Source/Editor/)
