#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
set(ENGINE_SOURCES
        src/HyperEngine/EngineLoop.cpp
        src/HyperEngine/IApplication.cpp
        src/HyperEngine/Launcher.cpp
        src/HyperEngine/Core/Logger.cpp
        src/HyperEngine/Platform/Window.cpp
        src/HyperEngine/Rendering/RenderContext.cpp)

set(ENGINE_HEADERS
        include/HyperEngine/EngineLoop.hpp
        include/HyperEngine/IApplication.hpp
        include/HyperEngine/Launcher.hpp
        include/HyperEngine/Core/Align.hpp
        include/HyperEngine/Core/Assertion.hpp
        include/HyperEngine/Core/Bits.hpp
        include/HyperEngine/Core/Compilers.hpp
        include/HyperEngine/Core/RenderingApi.hpp
        include/HyperEngine/Core/Hasher.hpp
        include/HyperEngine/Core/Logger.hpp
        include/HyperEngine/Core/Platform.hpp
        include/HyperEngine/Core/Prerequisites.hpp
        include/HyperEngine/Core/SourceLocation.hpp
        include/HyperEngine/Core/Compilers/ClangSpecific.hpp
        include/HyperEngine/Core/Compilers/GCCSpecific.hpp
        include/HyperEngine/Core/Compilers/MSVCSpecific.hpp
        include/HyperEngine/Core/Platform/LinuxSpecific.hpp
        include/HyperEngine/Core/Platform/WindowsSpecific.hpp
        include/HyperEngine/Platform/Window.hpp
        include/HyperEngine/Rendering/IContext.hpp
        include/HyperEngine/Rendering/IRenderer.hpp
        include/HyperEngine/Rendering/RenderContext.hpp
        include/HyperEngine/Math/Common.hpp
        include/HyperEngine/Math/Compare.hpp
        include/HyperEngine/Math/Constants.hpp
        include/HyperEngine/Math/Matrix2.hpp
        include/HyperEngine/Math/Matrix3.hpp
        include/HyperEngine/Math/Matrix4.hpp
        include/HyperEngine/Math/Quaternion.hpp
        include/HyperEngine/Math/Trigonometric.hpp
        include/HyperEngine/Math/Types.hpp
        include/HyperEngine/Math/Vector2.hpp
        include/HyperEngine/Math/Vector3.hpp
        include/HyperEngine/Math/Vector4.hpp)

if (HYPERENGINE_BUILD_OPENGL)
    set(OPENGL_SOURCES
            )

    set(OPENGL_HEADERS
            )
endif ()

if (HYPERENGINE_BUILD_VULKAN)
    set(VULKAN_SOURCES
            src/HyperEngine/Rendering/Vulkan/CommandBuffer.cpp
            src/HyperEngine/Rendering/Vulkan/Context.cpp
            src/HyperEngine/Rendering/Vulkan/Debug.cpp
            src/HyperEngine/Rendering/Vulkan/Device.cpp
            src/HyperEngine/Rendering/Vulkan/Fence.cpp
            src/HyperEngine/Rendering/Vulkan/FrameBuffer.cpp
            src/HyperEngine/Rendering/Vulkan/Renderer.cpp
            src/HyperEngine/Rendering/Vulkan/RenderPass.cpp
            src/HyperEngine/Rendering/Vulkan/Semaphore.cpp
            src/HyperEngine/Rendering/Vulkan/Surface.cpp
            src/HyperEngine/Rendering/Vulkan/Swapchain.cpp)

    set(VULKAN_HEADERS
            include/HyperEngine/Rendering/Vulkan/CommandBuffer.hpp
            include/HyperEngine/Rendering/Vulkan/Context.hpp
            include/HyperEngine/Rendering/Vulkan/Debug.hpp
            include/HyperEngine/Rendering/Vulkan/Device.hpp
            include/HyperEngine/Rendering/Vulkan/Fence.hpp
            include/HyperEngine/Rendering/Vulkan/FrameBuffer.hpp
            include/HyperEngine/Rendering/Vulkan/Renderer.hpp
            include/HyperEngine/Rendering/Vulkan/RenderPass.hpp
            include/HyperEngine/Rendering/Vulkan/Semaphore.hpp
            include/HyperEngine/Rendering/Vulkan/Surface.hpp
            include/HyperEngine/Rendering/Vulkan/Swapchain.hpp)
endif ()

set(SOURCES
        ${ENGINE_SOURCES}
        ${OPENGL_SOURCES}
        ${VULKAN_SOURCES})

set(HEADERS
        ${ENGINE_HEADERS}
        ${OPENGL_HEADERS}
        ${VULKAN_HEADERS})

hyperengine_create_library(HyperEngine)
target_link_libraries(
        HyperEngine
        PUBLIC
        CONAN_PKG::fmt
        CONAN_PKG::glfw
        CONAN_PKG::glslang)

if (HYPERENGINE_BUILD_OPENGL)
    target_compile_definitions(
            HyperEngine
            PRIVATE
            HYPERENGINE_BUILD_OPENGL=1)
    target_link_libraries(
            HyperEngine
            PUBLIC
            CONAN_PKG::glad)
endif ()

if (HYPERENGINE_BUILD_VULKAN)
    target_compile_definitions(
            HyperEngine
            PRIVATE
            HYPERENGINE_BUILD_VULKAN=1)
    target_link_libraries(
            HyperEngine
            PUBLIC
            CONAN_PKG::vulkan-headers
            CONAN_PKG::vulkan-memory-allocator
            CONAN_PKG::volk)
endif ()
