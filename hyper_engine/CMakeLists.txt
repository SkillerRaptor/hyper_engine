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
        src/drivers/vulkan/vulkan_render_conversion.cpp
        src/drivers/vulkan/vulkan_render_driver.cpp
        src/scene/resources/asset.cpp
        src/systems/input_system.cpp
        src/systems/render_system.cpp
        src/systems/window_system.cpp
        src/systems/render/camera.cpp
        src/systems/render/shader_compiler.cpp)

set(HEADERS
        include/engine.hpp
        include/core/assertion.hpp
        include/core/bit_flags.hpp
        include/core/filesystem.hpp
        include/core/logger.hpp
        include/core/prerequisites.hpp
        include/core/resource_id.hpp
        include/core/resource_owner.hpp
        include/core/string_utils.hpp
        include/drivers/vulkan/vulkan_render_conversion.hpp
        include/drivers/vulkan/vulkan_render_driver.hpp
        include/scene/resources/asset.hpp
        include/systems/input_system.hpp
        include/systems/render_system.hpp
        include/systems/window_system.hpp
        include/systems/render/camera.hpp
        include/systems/render/render_driver.hpp
        include/systems/render/render_types.hpp
        include/systems/render/resource_handle.hpp
        include/systems/render/shader_compiler.hpp
        include/systems/window/event_bus.hpp
        include/systems/window/event_handler.hpp
        include/systems/window/key_codes.hpp
        include/systems/window/key_events.hpp
        include/systems/window/mouse_codes.hpp
        include/systems/window/mouse_events.hpp
        include/systems/window/window_events.hpp)

#-------------------------------------------------------------------------------------------

link_libraries(
        DirectXShaderCompiler
        EnTT::EnTT
        fastgltf
        fmt
        glm
        imgui
        libassert::assert
        SDL3::SDL3
        spdlog
        stb
        TracyClient
        volk
        VulkanMemoryAllocator)
if (WIN32)
    link_libraries(
            d3d12.lib
            dxgi.lib
            D3D12MemoryAllocator)
endif ()

add_compile_definitions(
        -DGLM_FORCE_RADIANS
        -DGLM_FORCE_DEPTH_ZE_TO_ONE)

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
        -DCATCH_CONFIG_MAIN
        -DHE_TESTS=1)

hyperengine_define_executable(hyper_engine_profiling)
target_link_libraries(
        hyper_engine_profiling
        PRIVATE
        Catch2::Catch2)
target_compile_definitions(
        hyper_engine_tests
        PRIVATE
        -DTRACY_ENABLE
        -DHE_PROFILING=1)