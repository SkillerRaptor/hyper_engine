#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

set(SOURCES
        src/hyper_rhi/graphics_device.cpp
        src/hyper_rhi/vulkan/vulkan_buffer.cpp
        src/hyper_rhi/vulkan/vulkan_compute_pipeline.cpp
        src/hyper_rhi/vulkan/vulkan_conversion.cpp
        src/hyper_rhi/vulkan/vulkan_graphics_device.cpp
        src/hyper_rhi/vulkan/vulkan_pipeline_layout.cpp
        src/hyper_rhi/vulkan/vulkan_render_pipeline.cpp
        src/hyper_rhi/vulkan/vulkan_sampler.cpp
        src/hyper_rhi/vulkan/vulkan_shader.cpp
        src/hyper_rhi/vulkan/vulkan_texture.cpp
        src/hyper_rhi/vulkan/vulkan_texture_view.cpp)

set(HEADERS
        include/hyper_rhi/buffer.hpp
        include/hyper_rhi/compute_pipeline.hpp
        include/hyper_rhi/forward.hpp
        include/hyper_rhi/graphics_device.hpp
        include/hyper_rhi/pipeline_layout.hpp
        include/hyper_rhi/render_pipeline.hpp
        include/hyper_rhi/sampler.hpp
        include/hyper_rhi/shader.hpp
        include/hyper_rhi/texture.hpp
        include/hyper_rhi/texture_view.hpp
        include/hyper_rhi/types.hpp
        include/hyper_rhi/vulkan/vulkan_buffer.hpp
        include/hyper_rhi/vulkan/vulkan_compute_pipeline.hpp
        include/hyper_rhi/vulkan/vulkan_conversion.hpp
        include/hyper_rhi/vulkan/vulkan_graphics_device.hpp
        include/hyper_rhi/vulkan/vulkan_macros.hpp
        include/hyper_rhi/vulkan/vulkan_pipeline_layout.hpp
        include/hyper_rhi/vulkan/vulkan_render_pipeline.hpp
        include/hyper_rhi/vulkan/vulkan_sampler.hpp
        include/hyper_rhi/vulkan/vulkan_shader.hpp
        include/hyper_rhi/vulkan/vulkan_texture.hpp
        include/hyper_rhi/vulkan/vulkan_texture_view.hpp)

he_add_library(hyper_rhi)
target_link_libraries(
        hyper_rhi
        PRIVATE
        hyper_core
        hyper_platform
        DirectXShaderCompiler
        GPUOpen::VulkanMemoryAllocator
        SDL3::SDL3
        volk::volk)