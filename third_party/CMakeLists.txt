#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

include(FetchContent)

#-------------------------------------------------------------------------------------------
# Catch2 (v3.8.1)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        Catch2
        SYSTEM
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG 2b60af89e23d28eefc081bc930831ee9d45ea58b)

set(CATCH_INSTALL_DOCS OFF CACHE INTERNAL "")
set(CATCH_INSTALL_EXTRAS ON CACHE INTERNAL "")
set(CATCH_DEVELOPMENT_BUILD OFF CACHE INTERNAL "")
set(CATCH_ENABLE_REPRODUCIBLE_BUILD ON CACHE INTERNAL "")

FetchContent_MakeAvailable(Catch2)

#-------------------------------------------------------------------------------------------
# entt (v3.15.0)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        entt
        SYSTEM
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG d4014c74dc3793aba95ae354d6e23a026c2796db)

set(ENTT_USE_LIBCPP OFF CACHE INTERNAL "")
set(ENTT_USE_SANITIZER OFF CACHE INTERNAL "")
set(ENTT_USE_CLANG_TIDY OFF CACHE INTERNAL "")
set(ENTT_INCLUDE_HEADERS OFF CACHE INTERNAL "")
set(ENTT_INCLUDE_NATVIS OFF CACHE INTERNAL "")
set(ENTT_INSTALL OFF CACHE INTERNAL "")
set(ENTT_BUILD_TESTING OFF CACHE INTERNAL "")
set(ENTT_BUILD_TESTBED OFF CACHE INTERNAL "")
set(ENTT_BUILD_DOCS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(entt)

#-------------------------------------------------------------------------------------------
# D3D12MemoryAllocator (v3.0.1)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        D3D12MemoryAllocator
        SYSTEM
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
        GIT_TAG 9ef66bc14edd10dee0de3a545b98578363552f66)

set(D3D12MA_BUILD_SAMPLE OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(D3D12MemoryAllocator)

set_target_properties(
        D3D12MemoryAllocator
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# DirectXShaderCompiler (v1.8.2502)
#-------------------------------------------------------------------------------------------
hyperengine_download_and_extract(
        https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2505/dxc_2025_05_24.zip
        ${CMAKE_SOURCE_DIR}/third_party
        DirectXShaderCompiler)

add_library(DirectXShaderCompiler INTERFACE)
target_link_libraries(
        DirectXShaderCompiler
        INTERFACE
        ${CMAKE_SOURCE_DIR}/third_party/DirectXShaderCompiler/lib/x64/dxcompiler.lib)
target_include_directories(
        DirectXShaderCompiler
        SYSTEM
        INTERFACE
        ${CMAKE_SOURCE_DIR}/third_party/DirectXShaderCompiler/inc)

set_target_properties(
        DirectXShaderCompiler
        PROPERTIES
        FOLDER "third_party")

hyperengine_deploy_files(${CMAKE_CURRENT_LIST_DIR}/DirectXShaderCompiler/bin/x64/ hyper_engine/)

#-------------------------------------------------------------------------------------------
# fastgltf (v0.9.0)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        fastgltf
        SYSTEM
        GIT_REPOSITORY https://github.com/spnda/fastgltf.git
        GIT_TAG 0d1b67a28c4950ea2deb796702006dcbe31e02b3)

set(FASTGLTF_USE_CUSTOM_SMALLVECTOR OFF CACHE INTERNAL "")
set(FASTGLTF_ENABLE_TESTS OFF CACHE INTERNAL "")
set(FASTGLTF_ENABLE_EXAMPLES OFF CACHE INTERNAL "")
set(FASTGLTF_ENABLE_DOCS OFF CACHE INTERNAL "")
set(FASTGLTF_ENABLE_GLTF_RS OFF CACHE INTERNAL "")
set(FASTGLTF_ENABLE_ASSIMP OFF CACHE INTERNAL "")
set(FASTGLTF_ENABLE_DEPRECATED_EXT OFF CACHE INTERNAL "")
set(FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL OFF CACHE INTERNAL "")
set(FASTGLTF_USE_64BIT_FLOAT OFF CACHE INTERNAL "")
set(FASTGLTF_COMPILE_AS_CPP20 ON CACHE INTERNAL "")
set(FASTGLTF_ENABLE_CPP_MODULES OFF CACHE INTERNAL "")
set(FASTGLTF_USE_STD_MODULE OFF CACHE INTERNAL "")
set(FASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES OFF CACHE INTERNAL "")
set(FASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(fastgltf)

set_target_properties(
        fastgltf
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# fmt (v11.0.2)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        fmt
        SYSTEM
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 40626af88bd7df9a5fb80be7b25ac85b122d6c21)

set(FMT_PEDANTIC OFF CACHE INTERNAL "")
set(FMT_WERROR OFF CACHE INTERNAL "")
set(FMT_DOC OFF CACHE INTERNAL "")
set(FMT_INSTALL OFF CACHE INTERNAL "")
set(FMT_TEST OFF CACHE INTERNAL "")
set(FMT_FUZZ OFF CACHE INTERNAL "")
set(FMT_CUDA_TEST OFF CACHE INTERNAL "")
set(FMT_OS ON CACHE INTERNAL "")
set(FMT_MODULE OFF CACHE INTERNAL "")
set(FMT_SYSTEM_HEADERS OFF CACHE INTERNAL "")
set(FMT_UNICODE ON CACHE INTERNAL "")

FetchContent_MakeAvailable(fmt)

set_target_properties(
        fmt
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# glm (master - 07.02.2025)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        glm
        SYSTEM
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f)

set(GLM_BUILD_LIBRARY OFF CACHE INTERNAL "")
set(GLM_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLM_BUILD_INSTALL OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(glm)

target_compile_definitions(
        glm
        INTERFACE
        GLM_ENABLE_EXPERIMENTAL)

set_target_properties(
        glm
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# imgui (v1.92.1)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        imgui
        SYSTEM
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG 5d4126876bc10396d4c6511853ff10964414c776)

FetchContent_MakeAvailable(imgui)

set(SOURCES
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui.cpp
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui_demo.cpp
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui_draw.cpp
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui_tables.cpp
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui_widgets.cpp
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/imgui_impl_sdl3.cpp
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/imgui_impl_vulkan.cpp)

set(HEADERS
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imconfig.h
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui.h
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imgui_internal.h
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imstb_rectpack.h
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imstb_textedit.h
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/imstb_truetype.h
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/imgui_impl_sdl3.h
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/imgui_impl_vulkan.h)

if (WIN32)
    set(SOURCES
            ${SOURCES}
            ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/imgui_impl_dx12.cpp)

    set(HEADERS
            ${HEADERS}
            ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/imgui_impl_dx12.h)
endif ()

add_library(imgui ${SOURCES} ${HEADERS})
target_include_directories(
        imgui
        SYSTEM
        PUBLIC
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/
        ${CMAKE_BINARY_DIR}/_deps/imgui-src/backends/)
target_compile_definitions(
        imgui
        PUBLIC
        IMGUI_IMPL_VULKAN_USE_VOLK)
target_link_libraries(
        imgui
        PUBLIC
        SDL3::SDL3
        volk)

set_target_properties(
        imgui
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# meshoptimizer (v0.24)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        meshoptimizer
        SYSTEM
        GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
        GIT_TAG 7b2d4f4c817aea55d74dcd65d9763ac2ca608026)

set(MESHOPT_BUILD_DEMO OFF CACHE INTERNAL "")
set(MESHOPT_BUILD_GLTFPACK OFF CACHE INTERNAL "")
set(MESHOPT_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(MESHOPT_STABLE_EXPORTS OFF CACHE INTERNAL "")
set(MESHOPT_WERROR OFF CACHE INTERNAL "")
set(MESHOPT_INSTALL OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(meshoptimizer)

set_target_properties(
        meshoptimizer
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# SDL (v3.2.16)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        sdl
        SYSTEM
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG c9a6709bd21750f1ad9597be21abace78c6378c9)

set(SDL_SHARED OFF CACHE INTERNAL "")
set(SDL_STATIC ON CACHE INTERNAL "")
set(SDL_TEST_LIBRARY OFF CACHE INTERNAL "")
set(SDL_TESTS OFF CACHE INTERNAL "")
set(SDL_DISABLE_INSTALL ON CACHE INTERNAL "")
set(SDL_DISABLE_INSTALL_DOCS ON CACHE INTERNAL "")
set(SDL_INSTALL_TESTS OFF CACHE INTERNAL "")

set(SDL_AUDIO OFF CACHE INTERNAL "")
set(SDL_GPU OFF CACHE INTERNAL "")
set(SDL_RENDER OFF CACHE INTERNAL "")
set(SDL_CAMERA OFF CACHE INTERNAL "")
set(SDL_JOYSTICK OFF CACHE INTERNAL "")
set(SDL_HAPTIC OFF CACHE INTERNAL "")
set(SDL_HIDAPI OFF CACHE INTERNAL "")
set(SDL_POWER OFF CACHE INTERNAL "")
set(SDL_SENSOR OFF CACHE INTERNAL "")
set(SDL_DIALOG OFF CACHE INTERNAL "")

set(SDL_DUMMYVIDEO OFF CACHE INTERNAL "")
set(SDL_OFFSCREEN OFF CACHE INTERNAL "")
set(SDL_OPENGL OFF CACHE INTERNAL "")
set(SDL_OPENGLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(sdl)

set_target_properties(
        SDL3-static
        PROPERTIES
        FOLDER " third_party")
set_target_properties(
        SDL_uclibc
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# spdlog (v1.15.3)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        spdlog
        SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG 6fa36017cfd5731d617e1a934f0e5ea9c4445b13)

set(SPDLOG_BUILD_ALL OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_SHARED OFF CACHE INTERNAL "")
set(SPDLOG_ENABLE_PCH OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_PIC OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_EXAMPLE OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_EXAMPLE_HO OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_TESTS OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_TESTS_HO OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_BENCH OFF CACHE INTERNAL "")
set(SPDLOG_SANITIZE_ADDRESS OFF CACHE INTERNAL "")
set(SPDLOG_SANITIZE_THREAD OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_WARNINGS OFF CACHE INTERNAL "")
set(SPDLOG_SYSTEM_INCLUDES OFF CACHE INTERNAL "")
set(SPDLOG_INSTALL OFF CACHE INTERNAL "")
set(SPDLOG_USE_STD_FORMAT OFF CACHE INTERNAL "")
set(SPDLOG_FMT_EXTERNAL ON CACHE INTERNAL "")
set(SPDLOG_FMT_EXTERNAL_HO OFF CACHE INTERNAL "")
set(SPDLOG_NO_EXCEPTIONS OFF CACHE INTERNAL "")
set(SPDLOG_PREVENT_CHILD_FD OFF CACHE INTERNAL "")
set(SPDLOG_NO_THREAD_ID OFF CACHE INTERNAL "")
set(SPDLOG_NO_TLS OFF CACHE INTERNAL "")
set(SPDLOG_NO_ATOMIC_LEVELS OFF CACHE INTERNAL "")
set(SPDLOG_DISABLE_DEFAULT_LOGGER OFF CACHE INTERNAL "")
set(SPDLOG_FWRITE_UNLOCKED ON CACHE INTERNAL "")
set(SPDLOG_TIDY OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(spdlog)

set_target_properties(
        spdlog
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# stb
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        stb
        SYSTEM
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG 5c205738c191bcb0abc65c4febfa9bd25ff35234)

FetchContent_MakeAvailable(stb)

add_subdirectory(stb)

#-------------------------------------------------------------------------------------------
# tracy (v0.12.2)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        tracy
        SYSTEM
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG c556831ddc6fe26d2fce01c14c97205a9dad46d5)

set(TRACY_STATIC ON CACHE INTERNAL "")
set(TRACY_Fortran OFF CACHE INTERNAL "")
set(TRACY_LTO OFF CACHE INTERNAL "")
set(TRACY_ENABLE ON CACHE INTERNAL "")
set(TRACY_ON_DEMAND OFF CACHE INTERNAL "")
set(TRACY_CALLSTACK OFF CACHE INTERNAL "")
set(TRACY_NO_CALLSTACK OFF CACHE INTERNAL "")
set(TRACY_NO_CALLSTACK_INLINES OFF CACHE INTERNAL "")
set(TRACY_ONLY_LOCALHOST OFF CACHE INTERNAL "")
set(TRACY_NO_BROADCAST OFF CACHE INTERNAL "")
set(TRACY_ONLY_IPV4 OFF CACHE INTERNAL "")
set(TRACY_NO_CODE_TRANSFER OFF CACHE INTERNAL "")
set(TRACY_NO_CONTEXT_SWITCH OFF CACHE INTERNAL "")
set(TRACY_NO_EXIT OFF CACHE INTERNAL "")
set(TRACY_NO_SAMPLING OFF CACHE INTERNAL "")
set(TRACY_NO_VERIFY OFF CACHE INTERNAL "")
set(TRACY_NO_VSYNC_CAPTURE OFF CACHE INTERNAL "")
set(TRACY_NO_FRAME_IMAGE OFF CACHE INTERNAL "")
set(TRACY_NO_SYSTEM_TRACING OFF CACHE INTERNAL "")
set(TRACY_PATCHABLE_NOPSLEDS OFF CACHE INTERNAL "")
set(TRACY_DELAYED_INIT OFF CACHE INTERNAL "")
set(TRACY_MANUAL_LIFETIME OFF CACHE INTERNAL "")
set(TRACY_FIBERS OFF CACHE INTERNAL "")
set(TRACY_NO_CRASH_HANDLER OFF CACHE INTERNAL "")
set(TRACY_TIMER_FALLBACK OFF CACHE INTERNAL "")
set(TRACY_LIBUNWIND_BACKTRACE OFF CACHE INTERNAL "")
set(TRACY_SYMBOL_OFFLINE_RESOLVE OFF CACHE INTERNAL "")
set(TRACY_LIBBACKTRACE_ELF_DYNLOAD_SUPPORT OFF CACHE INTERNAL "")
set(TRACY_DEBUGINFOD OFF CACHE INTERNAL "")
set(TRACY_VERBOSE OFF CACHE INTERNAL "")
set(TRACY_DEMANGLE OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(tracy)

#-------------------------------------------------------------------------------------------
# volk (v1.4.304)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        volk
        SYSTEM
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG 0b17a763ba5643e32da1b2152f8140461b3b7345)

set(VOLK_PULL_IN_VULKAN ON CACHE INTERNAL "")
set(VOLK_INSTALL OFF CACHE INTERNAL "")
set(VOLK_HEADERS_ONLY OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(volk)

set_target_properties(
        volk
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# VulkanMemoryAllocator (v3.3.0)
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        VulkanMemoryAllocator
        SYSTEM
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG 1d8f600fd424278486eade7ed3e877c99f0846b1)

set(VMA_ENABLE_INSTALL OFF CACHE INTERNAL "")
set(VMA_BUILD_DOCUMENTATION OFF CACHE INTERNAL "")
set(VMA_BUILD_SAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(VulkanMemoryAllocator)

set_target_properties(
        VulkanMemoryAllocator
        PROPERTIES
        FOLDER "third_party")
