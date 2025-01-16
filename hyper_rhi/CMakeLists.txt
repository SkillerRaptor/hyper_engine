#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_rhi/buffer.cpp
        src/hyper_rhi/command_list.cpp
        src/hyper_rhi/compute_pass.cpp
        src/hyper_rhi/compute_pipeline.cpp
        src/hyper_rhi/descriptor_manager.cpp
        src/hyper_rhi/graphics_device.cpp
        src/hyper_rhi/pipeline_layout.cpp
        src/hyper_rhi/render_pass.cpp
        src/hyper_rhi/render_pipeline.cpp
        src/hyper_rhi/resource_handle.cpp
        src/hyper_rhi/sampler.cpp
        src/hyper_rhi/shader_compiler.cpp
        src/hyper_rhi/shader_module.cpp
        src/hyper_rhi/surface.cpp
        src/hyper_rhi/texture.cpp
        src/hyper_rhi/texture_view.cpp
        src/hyper_rhi/vulkan/vulkan_buffer.cpp
        src/hyper_rhi/vulkan/vulkan_command_list.cpp
        src/hyper_rhi/vulkan/vulkan_compute_pass.cpp
        src/hyper_rhi/vulkan/vulkan_compute_pipeline.cpp
        src/hyper_rhi/vulkan/vulkan_descriptor_manager.cpp
        src/hyper_rhi/vulkan/vulkan_graphics_device.cpp
        src/hyper_rhi/vulkan/vulkan_pipeline_layout.cpp
        src/hyper_rhi/vulkan/vulkan_render_pass.cpp
        src/hyper_rhi/vulkan/vulkan_render_pipeline.cpp
        src/hyper_rhi/vulkan/vulkan_sampler.cpp
        src/hyper_rhi/vulkan/vulkan_shader_module.cpp
        src/hyper_rhi/vulkan/vulkan_surface.cpp
        src/hyper_rhi/vulkan/vulkan_texture.cpp
        src/hyper_rhi/vulkan/vulkan_texture_view.cpp)

set(HEADERS
        include/hyper_rhi/buffer.hpp
        include/hyper_rhi/command_list.hpp
        include/hyper_rhi/compare_operation.hpp
        include/hyper_rhi/compute_pass.hpp
        include/hyper_rhi/compute_pipeline.hpp
        include/hyper_rhi/descriptor_manager.hpp
        include/hyper_rhi/dimension.hpp
        include/hyper_rhi/forward.hpp
        include/hyper_rhi/format.hpp
        include/hyper_rhi/graphics_device.hpp
        include/hyper_rhi/pipeline_layout.hpp
        include/hyper_rhi/render_pass.hpp
        include/hyper_rhi/render_pipeline.hpp
        include/hyper_rhi/resource_handle.hpp
        include/hyper_rhi/sampler.hpp
        include/hyper_rhi/shader_module.hpp
        include/hyper_rhi/shader_compiler.hpp
        include/hyper_rhi/shader_type.hpp
        include/hyper_rhi/subresource_range.hpp
        include/hyper_rhi/surface.hpp
        include/hyper_rhi/texture.hpp
        include/hyper_rhi/texture_view.hpp
        include/hyper_rhi/vulkan/vulkan_buffer.hpp
        include/hyper_rhi/vulkan/vulkan_command_list.hpp
        include/hyper_rhi/vulkan/vulkan_common.hpp
        include/hyper_rhi/vulkan/vulkan_compute_pass.hpp
        include/hyper_rhi/vulkan/vulkan_compute_pipeline.hpp
        include/hyper_rhi/vulkan/vulkan_descriptor_manager.hpp
        include/hyper_rhi/vulkan/vulkan_graphics_device.hpp
        include/hyper_rhi/vulkan/vulkan_pipeline_layout.hpp
        include/hyper_rhi/vulkan/vulkan_render_pass.hpp
        include/hyper_rhi/vulkan/vulkan_render_pipeline.hpp
        include/hyper_rhi/vulkan/vulkan_sampler.hpp
        include/hyper_rhi/vulkan/vulkan_shader_module.hpp
        include/hyper_rhi/vulkan/vulkan_surface.hpp
        include/hyper_rhi/vulkan/vulkan_texture.hpp
        include/hyper_rhi/vulkan/vulkan_texture_view.hpp)

if (WIN32)
    set(SOURCES
            ${SOURCES})

    set(HEADERS
            ${HEADERS}
            include/hyper_rhi/d3d12/d3d12_common.hpp)
endif ()

hyperengine_define_library(hyper_rhi)
target_link_libraries(
        hyper_rhi
        PUBLIC
        hyper_core
        DirectXShaderCompiler
        SDL3::SDL3
        volk
        VulkanMemoryAllocator)

if (WIN32)
    target_link_libraries(
            hyper_rhi
            PUBLIC
            d3d12.lib
            dxgi.lib
            D3D12MemoryAllocator)
endif ()
