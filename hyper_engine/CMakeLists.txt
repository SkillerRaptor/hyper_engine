#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

set(SOURCES
        src/engine.cpp
        src/main.cpp)

set(HEADERS
        include/engine.hpp)

he_define_executable(hyper_engine)
target_include_directories(
        hyper_engine
        PRIVATE
        ${CMAKE_SOURCE_DIR}/assets/shaders)

target_link_libraries(
        hyper_engine
        PRIVATE
        hyper_core
        hyper_platform
        hyper_rhi
        hyper_script
        fastgltf::fastgltf
        glm
        stb)

target_compile_definitions(
        hyper_engine
        PRIVATE
        GLM_FORCE_RADIANS
        GLM_FORCE_DEPTH_ZE_TO_ONE)

