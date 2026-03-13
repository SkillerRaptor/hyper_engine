#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
        NAME Catch2
        GITHUB_REPOSITORY catchorg/Catch2
        GIT_TAG v3.12.0
        OPTIONS
        "CATCH_INSTALL_DOCS OFF"
        "CATCH_INSTALL_EXTRAS ON"
        "CATCH_DEVELOPMENT_BUILD OFF"
        "CATCH_ENABLE_REPRODUCIBLE_BUILD ON")

he_systemize(Catch2)

CPMAddPackage(
        NAME entt
        GITHUB_REPOSITORY skypjack/entt
        GIT_TAG v3.16.0
        OPTIONS
        "ENTT_USE_LIBCPP OFF"
        "ENTT_USE_SANITIZER OFF"
        "ENTT_USE_CLANG_TIDY OFF"
        "ENTT_INCLUDE_HEADERS OFF"
        "ENTT_INCLUDE_NATVIS OFF"
        "ENTT_INSTALL OFF"
        "ENTT_BUILD_TESTING OFF"
        "ENTT_BUILD_TESTBED OFF"
        "ENTT_BUILD_DOCS OFF")

he_systemize(EnTT)

if (WIN32)
    CPMAddPackage(
            NAME D3D12MemoryAllocator
            GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator
            GIT_TAG v3.0.1
            OPTIONS
            "BUILD_DOCUMENTATION OFF"
            "BUILD_SHARED_LIBS OFF"
            "D3D12MA_BUILD_SAMPLE OFF")
endif ()

# he_systemize(D3D12MemoryAllocator)

CPMAddPackage(
        NAME fastgltf
        GITHUB_REPOSITORY spnda/fastgltf
        GIT_TAG v0.9.0
        OPTIONS
        "FASTGLTF_USE_CUSTOM_SMALLVECTOR OFF"
        "FASTGLTF_ENABLE_TESTS OFF"
        "FASTGLTF_ENABLE_EXAMPLES OFF"
        "FASTGLTF_ENABLE_DOCS OFF"
        "FASTGLTF_ENABLE_GLTF_RS OFF"
        "FASTGLTF_ENABLE_ASSIMP OFF"
        "FASTGLTF_ENABLE_DEPRECATED_EXT OFF"
        "FASTGLTF_DISABLE_CUSTOM_MEMORY_POOL OFF"
        "FASTGLTF_USE_64BIT_FLOAT OFF"
        "FASTGLTF_COMPILE_AS_CPP20 ON"
        "FASTGLTF_ENABLE_CPP_MODULES OFF"
        "FASTGLTF_USE_STD_MODULE OFF"
        "FASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES OFF"
        "FASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES OFF")

# he_systemize(fastgltf)

CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 5ac44cd1282f12947d25f439b59c5e55944cdb04
        OPTIONS
        "FMT_PEDANTIC OFF"
        "FMT_WERROR OFF"
        "FMT_DOC OFF"
        "FMT_INSTALL OFF"
        "FMT_TEST OFF"
        "FMT_FUZZ OFF"
        "FMT_CUDA_TEST OFF"
        "FMT_OS ON"
        "FMT_MODULE OFF"
        "FMT_SYSTEM_HEADERS OFF"
        "FMT_UNICODE ON")

he_systemize(fmt)

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG 1.0.3
        OPTIONS
        "GLM_BUILD_LIBRARY OFF"
        "GLM_BUILD_TESTS OFF"
        "GLM_BUILD_INSTALL OFF"
        "GLM_ENABLE_CXX_17 OFF"
        "GLM_ENABLE_CXX_20 ON"
        "GLM_ENABLE_LANG_EXTENSIONS OFF"
        "GLM_DISABLE_AUTO_DETECTION OFF"
        "GLM_ENABLE_FAST_MATH OFF"
        "GLM_ENABLE_SIMD_SSE2 OFF"
        "GLM_ENABLE_SIMD_SSE3 OFF"
        "GLM_ENABLE_SIMD_SSSE3 OFF"
        "GLM_ENABLE_SIMD_SSE4_1 OFF"
        "GLM_ENABLE_SIMD_SSE4_2 OFF"
        "GLM_ENABLE_SIMD_AVX OFF"
        "GLM_ENABLE_SIMD_AVX2 OFF"
        "GLM_ENABLE_SIMD_NEON OFF"
        "GLM_FORCE_PURE OFF")

he_systemize(glm)

CPMAddPackage(
        NAME imgui
        GITHUB_REPOSITORY ocornut/imgui
        GIT_TAG v1.92.5
        DOWNLOAD_ONLY YES)

if (imgui_ADDED)
    add_subdirectory(imgui)
endif ()

he_systemize(imgui)

CPMAddPackage(
        NAME meshoptimizer
        GITHUB_REPOSITORY zeux/meshoptimizer
        GIT_TAG v1.0
        OPTIONS
        "MESHOPT_BUILD_DEMO OFF"
        "MESHOPT_BUILD_GLTFPACK OFF"
        "MESHOPT_BUILD_SHARED_LIBS OFF"
        "MESHOPT_STABLE_EXPORTS OFF"
        "MESHOPT_WERROR OFF"
        "MESHOPT_INSTALL OFF")

he_systemize(meshoptimizer)

CPMAddPackage(
        NAME SDL
        GITHUB_REPOSITORY libsdl-org/SDL
        GIT_TAG release-3.4.0
        OPTIONS
        "SDL_SHARED OFF"
        "SDL_STATIC ON"
        "SDL_TEST_LIBRARY OFF"
        "SDL_TESTS OFF"
        "SDL_DISABLE_INSTALL ON"
        "SDL_DISABLE_INSTALL_DOCS ON"
        "SDL_INSTALL_TESTS OFF"
        "SDL_AUDIO OFF"
        "SDL_GPU OFF"
        "SDL_RENDER OFF"
        "SDL_CAMERA OFF"
        "SDL_JOYSTICK OFF"
        "SDL_HAPTIC OFF"
        "SDL_HIDAPI OFF"
        "SDL_POWER OFF"
        "SDL_SENSOR OFF"
        "SDL_DIALOG OFF"
        "SDL_DUMMYVIDEO OFF"
        "SDL_OFFSCREEN OFF"
        "SDL_OPENGL OFF"
        "SDL_OPENGLES OFF")

he_systemize(SDL3-static)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        GIT_TAG v1.17.0
        OPTIONS
        "SPDLOG_BUILD_ALL OFF"
        "SPDLOG_BUILD_SHARED OFF"
        "SPDLOG_ENABLE_PCH OFF"
        "SPDLOG_BUILD_PIC OFF"
        "SPDLOG_BUILD_EXAMPLE OFF"
        "SPDLOG_BUILD_EXAMPLE_HO OFF"
        "SPDLOG_BUILD_TESTS OFF"
        "SPDLOG_BUILD_TESTS_HO OFF"
        "SPDLOG_BUILD_BENCH OFF"
        "SPDLOG_SANITIZE_ADDRESS OFF"
        "SPDLOG_SANITIZE_THREAD OFF"
        "SPDLOG_BUILD_WARNINGS OFF"
        "SPDLOG_SYSTEM_INCLUDES OFF"
        "SPDLOG_INSTALL OFF"
        "SPDLOG_USE_STD_FORMAT OFF"
        "SPDLOG_FMT_EXTERNAL ON"
        "SPDLOG_FMT_EXTERNAL_HO OFF"
        "SPDLOG_NO_EXCEPTIONS OFF"
        "SPDLOG_PREVENT_CHILD_FD OFF"
        "SPDLOG_NO_THREAD_ID OFF"
        "SPDLOG_NO_TLS OFF"
        "SPDLOG_NO_ATOMIC_LEVELS OFF"
        "SPDLOG_DISABLE_DEFAULT_LOGGER OFF"
        "SPDLOG_FWRITE_UNLOCKED ON"
        "SPDLOG_TIDY OFF")

he_systemize(spdlog)

CPMAddPackage(
        NAME stb
        GITHUB_REPOSITORY nothings/stb
        GIT_TAG f1c79c02822848a9bed4315b12c8c8f3761e1296
        DOWNLOAD_ONLY YES)

if (stb_ADDED)
    add_subdirectory(stb)
endif ()

he_systemize(stb)

CPMAddPackage(
        NAME tracy
        GITHUB_REPOSITORY wolfpld/tracy
        GIT_TAG v0.13.1
        OPTIONS
        "TRACY_STATIC ON"
        "TRACY_Fortran OFF"
        "TRACY_LTO OFF"
        "TRACY_ENABLE ON"
        "TRACY_ON_DEMAND OFF"
        "TRACY_CALLSTACK OFF"
        "TRACY_NO_CALLSTACK OFF"
        "TRACY_NO_CALLSTACK_INLINES OFF"
        "TRACY_ONLY_LOCALHOST OFF"
        "TRACY_NO_BROADCAST OFF"
        "TRACY_ONLY_IPV4 OFF"
        "TRACY_NO_CODE_TRANSFER OFF"
        "TRACY_NO_CONTEXT_SWITCH OFF"
        "TRACY_NO_EXIT OFF"
        "TRACY_NO_SAMPLING OFF"
        "TRACY_NO_VERIFY OFF"
        "TRACY_NO_VSYNC_CAPTURE OFF"
        "TRACY_NO_FRAME_IMAGE OFF"
        "TRACY_NO_SYSTEM_TRACING OFF"
        "TRACY_PATCHABLE_NOPSLEDS OFF"
        "TRACY_DELAYED_INIT OFF"
        "TRACY_MANUAL_LIFETIME OFF"
        "TRACY_FIBERS OFF"
        "TRACY_NO_CRASH_HANDLER OFF"
        "TRACY_TIMER_FALLBACK OFF"
        "TRACY_LIBUNWIND_BACKTRACE OFF"
        "TRACY_SYMBOL_OFFLINE_RESOLVE OFF"
        "TRACY_LIBBACKTRACE_ELF_DYNLOAD_SUPPORT OFF"
        "TRACY_DEBUGINFOD OFF"
        "TRACY_VERBOSE OFF"
        "TRACY_DEMANGLE OFF")

# he_systemize(TracyClient)

CPMAddPackage(
        NAME volk
        GITHUB_REPOSITORY zeux/volk
        GIT_TAG 1.4.304
        OPTIONS
        "VOLK_PULL_IN_VULKAN ON"
        "VOLK_INSTALL OFF"
        "VOLK_HEADERS_ONLY OFF")

he_systemize(volk)

CPMAddPackage(
        NAME VulkanMemoryAllocator
        GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG v3.3.0
        OPTIONS
        "VMA_ENABLE_INSTALL OFF"
        "VMA_BUILD_DOCUMENTATION OFF"
        "VMA_BUILD_SAMPLES OFF")

he_systemize(VulkanMemoryAllocator)

he_download_and_extract(
        https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.9.2602/dxc_2026_02_20.zip
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

he_deploy_files(${CMAKE_CURRENT_LIST_DIR}/DirectXShaderCompiler/bin/x64/ hyper_engine/)