#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperEngine/EngineLoop.cpp
        src/HyperEngine/IApplication.cpp
        src/HyperEngine/Launcher.cpp
        src/HyperEngine/Core/Logger.cpp
        src/HyperEngine/Platform/Window.cpp)

set(HEADERS
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

hyperengine_create_library(HyperEngine)
target_link_libraries(
        HyperEngine
        PUBLIC
        CONAN_PKG::fmt
        CONAN_PKG::glad
        CONAN_PKG::glfw
        CONAN_PKG::glslang
        CONAN_PKG::vulkan-headers
        CONAN_PKG::vulkan-memory-allocator
        CONAN_PKG::volk)

if (HYPERENGINE_BUILD_OPENGL)
    target_compile_definitions(
            HyperEngine
            PRIVATE
            HYPERENGINE_BUILD_OPENGL=1)
endif ()

if (HYPERENGINE_BUILD_VULKAN)
    target_compile_definitions(
            HyperEngine
            PRIVATE
            HYPERENGINE_BUILD_VULKAN=1)
endif ()
