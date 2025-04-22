#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

include(FetchContent)

#-------------------------------------------------------------------------------------------
# entt
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        entt
        SYSTEM
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG 4a2d1a8541228a90e02a873dba6a980506c42c03)

set(ENTT_USE_LIBCPP OFF CACHE INTERNAL "")
set(ENTT_USE_SANITIZER OFF CACHE INTERNAL "")
set(ENTT_USE_CLANG_TIDY OFF CACHE INTERNAL "")
set(ENTT_INCLUDE_HEADERS OFF CACHE INTERNAL "")
set(ENTT_INCLUDE_NATVIS OFF CACHE INTERNAL "")
set(ENTT_BUILD_TESTING OFF CACHE INTERNAL "")
set(ENTT_BUILD_TOOLS OFF CACHE INTERNAL "")
set(ENTT_BUILD_DOCS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(entt)

#-------------------------------------------------------------------------------------------
# D3D12MemoryAllocator
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        D3D12MemoryAllocator
        SYSTEM
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git
        GIT_TAG da380f69bd4547cd776c525ae225bb9d13df94e2)

set(D3D12MA_BUILD_SAMPLE OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(D3D12MemoryAllocator)

set_target_properties(
        D3D12MemoryAllocator
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# DirectXShaderCompiler
#-------------------------------------------------------------------------------------------
hyperengine_download_and_extract(
        https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2407/dxc_2024_07_31.zip
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
# fastgltf
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        fastgltf
        SYSTEM
        GIT_REPOSITORY https://github.com/spnda/fastgltf.git
        GIT_TAG d4734a7e55d8d25155e755d895e018e423845cd0)

set(FASTGLTF_USE_CUSTOM_SMALLVECTOR ON CACHE INTERNAL "")
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

FetchContent_MakeAvailable(fastgltf)

set_target_properties(
        fastgltf
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        fmt
        SYSTEM
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 0c9fce2ffefecfdce794e1859584e25877b7b592)

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
# glm
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        glm
        SYSTEM
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863)

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
# imgui
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        imgui
        SYSTEM
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG eb642a9535055eacca6c6f7f7310fb5b41a96424)

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
# libassert
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        libassert
        SYSTEM
        GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
        GIT_TAG bee90cbbcbe7533b2ccfe8c1705f42010dbfeb92)

FetchContent_MakeAvailable(libassert)

target_compile_definitions(libassert-lib PUBLIC LIBASSERT_PREFIX_ASSERTIONS)

set_target_properties(
        cpptrace-lib
        PROPERTIES
        FOLDER "third_party")
set_target_properties(
        libassert-lib
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# meshoptimizer
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        meshoptimizer
        SYSTEM
        GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
        GIT_TAG 4affad044571506a5724c9a6f15424f43e86f731)

set(MESHOPT_BUILD_DEMO OFF CACHE INTERNAL "")
set(MESHOPT_BUILD_GLTFPACK OFF CACHE INTERNAL "")
set(MESHOPT_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(MESHOPT_WERROR OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(meshoptimizer)

set_target_properties(
        meshoptimizer
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# SDL
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        sdl
        SYSTEM
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG f6864924f76e1a0b4abaefc76ae2ed22b1a8916e)

set(SDL_SHARED OFF CACHE INTERNAL "")
set(SDL_STATIC ON CACHE INTERNAL "")
set(SDL_TEST_LIBRARY OFF CACHE INTERNAL "")
set(SDL_TESTS OFF CACHE INTERNAL "")
set(SDL_DISABLE_INSTALL ON CACHE INTERNAL "")
set(SDL_DISABLE_INSTALL_DOCS ON CACHE INTERNAL "")
set(SDL_INSTALL_TESTS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(sdl)

set_target_properties(
        SDL3-static
        PROPERTIES
        FOLDER "third_party")
set_target_properties(
        SDL_uclibc
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# spdlog
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        spdlog
        SYSTEM
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG 8e5613379f5140fefb0b60412fbf1f5406e7c7f8)

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
# volk
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        volk
        SYSTEM
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG b465507a1720ea32d65163153a53ffa136ca835f)

set(VOLK_PULL_IN_VULKAN ON CACHE INTERNAL "")
set(VOLK_INSTALL OFF CACHE INTERNAL "")
set(VOLK_HEADERS_ONLY OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(volk)

set_target_properties(
        volk
        PROPERTIES
        FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# VulkanMemoryAllocator
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
        VulkanMemoryAllocator
        SYSTEM
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG 3bab6924988e5f19bf36586a496156cf72f70d9f)

set(VMA_ENABLE_INSTALL OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(VulkanMemoryAllocator)

set_target_properties(
        VulkanMemoryAllocator
        PROPERTIES
        FOLDER "third_party")
