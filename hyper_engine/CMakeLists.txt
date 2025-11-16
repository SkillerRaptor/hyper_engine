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
        src/platform/event_server.cpp
        src/platform/input_server.cpp
        src/platform/window_server.cpp
        src/render/camera.cpp
        src/render/render_driver.cpp
        src/render/render_server.cpp
        src/render/shader_compiler.cpp
        src/render/vulkan/vulkan_render_conversion.cpp
        src/render/vulkan/vulkan_render_driver.cpp
        src/scene/resources/asset.cpp)

set(HEADERS
        include/engine.hpp
        include/core/assertion.hpp
        include/core/bit_flags.hpp
        include/core/filesystem.hpp
        include/core/logger.hpp
        include/core/memory.hpp
        include/core/prerequisites.hpp
        include/core/result.hpp
        include/core/string_utils.hpp
        include/core/types.hpp
        include/platform/event_server.hpp
        include/platform/input_server.hpp
        include/platform/window_server.hpp
        include/platform/key_codes.hpp
        include/platform/key_events.hpp
        include/platform/mouse_codes.hpp
        include/platform/mouse_events.hpp
        include/platform/window_events.hpp
        include/render/camera.hpp
        include/render/definitions.hpp
        include/render/render_driver.hpp
        include/render/render_server.hpp
        include/render/resource_handle.hpp
        include/render/shader_compiler.hpp
        include/render/vulkan/vulkan_render_conversion.hpp
        include/render/vulkan/vulkan_render_driver.hpp
        include/scene/resources/asset.hpp)

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
