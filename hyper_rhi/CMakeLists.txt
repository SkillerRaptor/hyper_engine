#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_rhi/graphics_device.cpp
        src/hyper_rhi/surface.cpp
        src/hyper_rhi/vulkan/graphics_device.cpp
        src/hyper_rhi/vulkan/surface.cpp)

set(HEADERS
        include/hyper_rhi/graphics_device.hpp
        include/hyper_rhi/surface.hpp
        include/hyper_rhi/vulkan/common.hpp
        include/hyper_rhi/vulkan/graphics_device.hpp
        include/hyper_rhi/vulkan/surface.hpp)

if (WIN32)
    set(SOURCES
            ${SOURCES}
            src/hyper_rhi/d3d12/graphics_device.cpp
            src/hyper_rhi/d3d12/surface.cpp)

    set(HEADERS
            ${HEADERS}
            include/hyper_rhi/d3d12/common.hpp
            include/hyper_rhi/d3d12/graphics_device.hpp
            include/hyper_rhi/d3d12/surface.hpp)
endif ()

hyperengine_define_library(hyper_rhi)
target_link_libraries(
        hyper_rhi
        PUBLIC
        hyper_core
        hyper_platform
        glfw
        volk)

if (WIN32)
    target_link_libraries(
            hyper_rhi
            PUBLIC
            d3d12.lib
            dxgi.lib)
    target_compile_definitions(
            hyper_rhi
            PUBLIC
            NOMINMAX
            WIN32_LEAN_AND_MEAN)
endif ()
