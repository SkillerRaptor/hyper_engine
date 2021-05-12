#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#-------------------------------------------------------------------------------------------
# HyperResource
#-------------------------------------------------------------------------------------------
file(GLOB_RECURSE "SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE "HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")
add_library(HyperResource STATIC "${SOURCES}" "${HEADERS}")

target_include_directories(HyperResource PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries(HyperResource PUBLIC assimp)
target_include_directories(HyperResource PUBLIC "${CMAKE_SOURCE_DIR}/vendor/assimp/include/")

target_link_libraries(HyperResource PUBLIC dxcompiler)
target_include_directories(HyperResource PUBLIC "${CMAKE_SOURCE_DIR}/vendor/DirectXShaderCompiler/include/")

target_link_libraries(HyperResource PUBLIC meshoptimizer)
target_include_directories(HyperResource PUBLIC "${CMAKE_SOURCE_DIR}/vendor/meshoptimizer/src/")

target_link_libraries(HyperResource PUBLIC spirv-cross-core)
target_link_libraries(HyperResource PUBLIC spirv-cross-cpp)
target_link_libraries(HyperResource PUBLIC spirv-cross-glsl)
target_link_libraries(HyperResource PUBLIC spirv-cross-hlsl)
target_link_libraries(HyperResource PUBLIC spirv-cross-reflect)
target_include_directories(HyperResource PUBLIC "${CMAKE_SOURCE_DIR}/vendor/SPIRV-Cross/")

target_link_libraries(HyperResource PUBLIC stb_image)
target_include_directories(HyperResource PUBLIC "${CMAKE_SOURCE_DIR}/vendor/stb_image/include/")

hyperengine_set_output_directories(HyperResource)
