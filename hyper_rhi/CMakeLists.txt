#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_rhi/graphics_device.cpp
        src/hyper_rhi/resource_handle.cpp
        src/hyper_rhi/vulkan/vulkan_command_list.cpp
        src/hyper_rhi/vulkan/vulkan_descriptor_manager.cpp
        src/hyper_rhi/vulkan/vulkan_graphics_device.cpp
        src/hyper_rhi/vulkan/vulkan_surface.cpp)

set(HEADERS
        include/hyper_rhi/buffer.hpp
        include/hyper_rhi/command_list.hpp
        include/hyper_rhi/compute_pipeline.hpp
        include/hyper_rhi/graphics_device.hpp
        include/hyper_rhi/graphics_pipeline.hpp
        include/hyper_rhi/pipeline_layout.hpp
        include/hyper_rhi/render_pass.hpp
        include/hyper_rhi/resource_handle.hpp
        include/hyper_rhi/shader_module.hpp
        include/hyper_rhi/surface.hpp
        include/hyper_rhi/texture.hpp
        include/hyper_rhi/vulkan/vulkan_command_list.hpp
        include/hyper_rhi/vulkan/vulkan_common.hpp
        include/hyper_rhi/vulkan/vulkan_descriptor_manager.hpp
        include/hyper_rhi/vulkan/vulkan_graphics_device.hpp
        include/hyper_rhi/vulkan/vulkan_surface.hpp)

if (WIN32)
    set(SOURCES
            ${SOURCES}
            src/hyper_rhi/d3d12/d3d12_graphics_device.cpp
            src/hyper_rhi/d3d12/d3d12_surface.cpp)

    set(HEADERS
            ${HEADERS}
            include/hyper_rhi/d3d12/d3d12_common.hpp
            include/hyper_rhi/d3d12/d3d12_graphics_device.hpp
            include/hyper_rhi/d3d12/d3d12_surface.hpp)
endif ()

hyperengine_define_library(hyper_rhi)
target_link_libraries(
        hyper_rhi
        PUBLIC
        hyper_core
        hyper_platform
        DirectXShaderCompiler
        glfw
        volk
        VulkanMemoryAllocator)

if (WIN32)
    target_link_libraries(
            hyper_rhi
            PUBLIC
            d3d12.lib
            dxgi.lib
            D3D12MemoryAllocator)
    target_compile_definitions(
            hyper_rhi
            PUBLIC
            NOMINMAX
            WIN32_LEAN_AND_MEAN)
endif ()
