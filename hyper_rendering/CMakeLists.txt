#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/hyper_rendering/material.cpp
        src/hyper_rendering/mesh.cpp
        src/hyper_rendering/renderable.cpp
        src/hyper_rendering/renderer.cpp
        src/hyper_rendering/scene.cpp
        src/hyper_rendering/render_passes/grid_pass.cpp
        src/hyper_rendering/render_passes/opaque_pass.cpp)

set(HEADERS
        include/hyper_rendering/camera.hpp
        include/hyper_rendering/forward.hpp
        include/hyper_rendering/material.hpp
        include/hyper_rendering/mesh.hpp
        include/hyper_rendering/renderable.hpp
        include/hyper_rendering/renderer.hpp
        include/hyper_rendering/scene.hpp
        include/hyper_rendering/render_passes/grid_pass.hpp
        include/hyper_rendering/render_passes/opaque_pass.hpp)

hyperengine_define_library(hyper_rendering)
target_include_directories(
        hyper_rendering
        PRIVATE
        ${CMAKE_SOURCE_DIR}/assets/shaders)
target_link_libraries(
        hyper_rendering
        PUBLIC
        hyper_core
        hyper_rhi
        EnTT::EnTT
        fastgltf
        glm
        imgui
        stb)

