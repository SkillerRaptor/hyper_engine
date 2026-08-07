#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

#-------------------------------------------------------------------------------------------
# Catch2
#-------------------------------------------------------------------------------------------
CPMAddPackage(
        NAME Catch2
        GITHUB_REPOSITORY catchorg/Catch2
        GIT_TAG v3.12.0
        OPTIONS
        "CATCH_INSTALL_DOCS OFF"
        "CATCH_INSTALL_EXTRAS ON"
        "CATCH_DEVELOPMENT_BUILD OFF"
        "CATCH_ENABLE_REPRODUCIBLE_BUILD ON")

#-------------------------------------------------------------------------------------------
# DirectXShaderCompiler
#-------------------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------------------
# fastgltf
#-------------------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------------------
# glm
#-------------------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------------------
# meshoptimizer
#-------------------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------------------
# SDL
#-------------------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------------------
# stb
#-------------------------------------------------------------------------------------------
CPMAddPackage(
        NAME stb
        GITHUB_REPOSITORY nothings/stb
        GIT_TAG f1c79c02822848a9bed4315b12c8c8f3761e1296
        DOWNLOAD_ONLY YES)

if (stb_ADDED)
    add_subdirectory(stb)
endif ()

#-------------------------------------------------------------------------------------------
# volk
#-------------------------------------------------------------------------------------------
CPMAddPackage(
        NAME volk
        GITHUB_REPOSITORY zeux/volk
        GIT_TAG 1.4.304
        OPTIONS
        "VOLK_PULL_IN_VULKAN ON"
        "VOLK_INSTALL OFF"
        "VOLK_HEADERS_ONLY OFF")

#-------------------------------------------------------------------------------------------
# VulkanMemoryAllocator
#-------------------------------------------------------------------------------------------
CPMAddPackage(
        NAME VulkanMemoryAllocator
        GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG v3.3.0
        OPTIONS
        "VMA_ENABLE_INSTALL OFF"
        "VMA_BUILD_DOCUMENTATION OFF"
        "VMA_BUILD_SAMPLES OFF")