#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/engine.cpp
        src/main.cpp
        src/core/filesystem.cpp
        src/core/logger.cpp
        src/core/string_utils.cpp
        src/platform/input.cpp
        src/platform/window.cpp
        src/rhi/graphics_device.cpp
        src/rhi/validation.cpp
        src/rhi/vulkan/buffer.cpp
        src/rhi/vulkan/compute_pipeline.cpp
        src/rhi/vulkan/graphics_device.cpp
        src/rhi/vulkan/pipeline_layout.cpp
        src/rhi/vulkan/render_conversion.cpp
        src/rhi/vulkan/render_pipeline.cpp
        src/rhi/vulkan/sampler.cpp
        src/rhi/vulkan/shader.cpp
        src/rhi/vulkan/texture.cpp)

set(HEADERS
        include/engine.hpp
        include/core/assertion.hpp
        include/core/bit_flags.hpp
        include/core/filesystem.hpp
        include/core/logger.hpp
        include/core/memory.hpp
        include/core/prerequisites.hpp
        include/core/string_utils.hpp
        include/core/types.hpp
        include/platform/input.hpp
        include/platform/key_codes.hpp
        include/platform/mouse_codes.hpp
        include/platform/window.hpp
        include/rhi/buffer.hpp
        include/rhi/compute_pipeline.hpp
        include/rhi/definitions.hpp
        include/rhi/graphics_device.hpp
        include/rhi/pipeline_layout.hpp
        include/rhi/render_pipeline.hpp
        include/rhi/resource_handle.hpp
        include/rhi/sampler.hpp
        include/rhi/shader.hpp
        include/rhi/texture.hpp
        include/rhi/validation.hpp
        include/rhi/vulkan/buffer.hpp
        include/rhi/vulkan/compute_pipeline.hpp
        include/rhi/vulkan/graphics_device.hpp
        include/rhi/vulkan/pipeline_layout.hpp
        include/rhi/vulkan/render_conversion.hpp
        include/rhi/vulkan/render_pipeline.hpp
        include/rhi/vulkan/sampler.hpp
        include/rhi/vulkan/shader.hpp
        include/rhi/vulkan/texture.hpp)

#-------------------------------------------------------------------------------------------
link_libraries(
        DirectXShaderCompiler
        EnTT::EnTT
        fastgltf
        fmt
        glm
        imgui
        SDL3::SDL3
        spdlog
        stb
        TracyClient
        volk
        VulkanMemoryAllocator)
if (WIN32)
    link_libraries(d3d12.lib dxgi.lib D3D12MemoryAllocator)
endif ()

add_compile_definitions(GLM_FORCE_RADIANS GLM_FORCE_DEPTH_ZE_TO_ONE)
include_directories(${CMAKE_SOURCE_DIR}/assets/shaders)

#-------------------------------------------------------------------------------------------

hyperengine_define_executable(hyper_engine)
target_link_libraries(
        hyper_engine
        PRIVATE
        Catch2::Catch2)

hyperengine_define_executable(hyper_engine_tests)
target_link_libraries(
        hyper_engine_tests
        PRIVATE
        Catch2::Catch2WithMain)
target_compile_definitions(
        hyper_engine_tests
        PRIVATE
        CATCH_CONFIG_MAIN
        HE_TESTS=1)

hyperengine_define_executable(hyper_engine_profiling)
target_link_libraries(
        hyper_engine_profiling
        PRIVATE
        Catch2::Catch2)
target_compile_definitions(
        hyper_engine_tests
        PRIVATE
        TRACY_ENABLE
        HE_PROFILING=1)
