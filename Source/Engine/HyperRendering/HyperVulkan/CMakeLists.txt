#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperVulkan
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(PLATFORM_SOURCES
            src/HyperRendering/Vulkan/Windows/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            include/HyperRendering/Vulkan/Windows/PlatformContext.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(PLATFORM_SOURCES
            src/HyperRendering/Vulkan/Linux/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            include/HyperRendering/Vulkan/Linux/PlatformContext.hpp)
endif ()

set(CORE_SOURCES
        src/HyperRendering/Vulkan/Context.cpp
        src/HyperRendering/Vulkan/Device.cpp
        src/HyperRendering/Vulkan/IPlatformContext.cpp
        src/HyperRendering/Vulkan/Renderer.cpp
        src/HyperRendering/Vulkan/SwapChain.cpp)

set(CORE_HEADERS
        include/HyperRendering/Vulkan/Context.hpp
        include/HyperRendering/Vulkan/Device.hpp
        include/HyperRendering/Vulkan/IPlatformContext.hpp
        include/HyperRendering/Vulkan/Renderer.hpp
        include/HyperRendering/Vulkan/SwapChain.hpp)

set(SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES} ${HYPERCOMMON_SOURCES})
set(HEADERS ${CORE_HEADERS} ${PLATFORM_HEADERS} ${HYPERCOMMON_HEADERS})

group_source()

define_module(HyperVulkan)
target_include_directories(HyperVulkan PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(HyperVulkan PUBLIC HyperCore)
target_link_libraries(HyperVulkan PUBLIC HyperPlatform)
target_link_libraries(HyperVulkan PUBLIC HyperRenderingBase)
target_link_libraries(HyperVulkan PRIVATE ${Vulkan_LIBRARIES})
