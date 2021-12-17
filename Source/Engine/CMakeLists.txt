#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperEngine/Application.cpp
        src/HyperEngine/EngineLoop.cpp
        src/HyperEngine/Rendering/Device.cpp
        src/HyperEngine/Rendering/RenderContext.cpp
        src/HyperEngine/Rendering/SwapChain.cpp
        src/HyperEngine/Rendering/Utils.cpp
        src/HyperEngine/Platform/Window.cpp
        src/HyperEngine/Support/Error.cpp
        src/HyperEngine/Support/ErrorHandling.cpp)

set(HEADERS
        include/HyperEngine/Application.hpp
        include/HyperEngine/EngineLoop.hpp
        include/HyperEngine/Formatter.hpp
        include/HyperEngine/Launcher.hpp
        include/HyperEngine/Logger.hpp
        include/HyperEngine/Rendering/Device.hpp
        include/HyperEngine/Rendering/RenderContext.hpp
        include/HyperEngine/Rendering/SwapChain.hpp
        include/HyperEngine/Rendering/Utils.hpp
        include/HyperEngine/Platform/Window.hpp
        include/HyperEngine/Support/Compiler.hpp
        include/HyperEngine/Support/Error.hpp
        include/HyperEngine/Support/ErrorHandling.hpp
        include/HyperEngine/Support/Expected.hpp
        include/HyperEngine/Support/Prerequisites.hpp)

add_library(HyperEngine STATIC ${SOURCES} ${HEADERS})
target_link_libraries(
        HyperEngine
        PRIVATE
        ProjectOptions
        ProjectWarnings)
target_include_directories(
        HyperEngine
        PUBLIC
        include)
target_link_libraries(
        HyperEngine
        PUBLIC
        glfw
        Vulkan-Headers
        volk)
