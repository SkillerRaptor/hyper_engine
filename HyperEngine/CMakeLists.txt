#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
file(GLOB_RECURSE "C_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "C_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE "CXX_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE "CXX_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

set(SOURCES ${C_SOURCES} ${CXX_SOURCES})
set(HEADERS ${C_HEADERS} ${CXX_HEADERS})

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")
add_library(HyperEngine ${SOURCES} ${HEADERS})

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/HyperEngine/src/HyperEngine/)
target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/HyperEngine/src/HyperEngine/HyperCommon/)

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/DirectXShaderCompiler/include/)
target_link_libraries(HyperEngine PUBLIC dxcompiler)

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/fmt/include/)
target_link_libraries(HyperEngine PUBLIC fmt)

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include/)
target_link_libraries(HyperEngine PUBLIC glad)

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glfw/include/)
target_link_libraries(HyperEngine PUBLIC glfw)

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glm/)
target_link_libraries(HyperEngine PUBLIC glm)

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/json/include/)
target_link_libraries(HyperEngine PUBLIC nlohmann_json)

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/rttr/src/)
target_link_libraries(HyperEngine PUBLIC rttr_core_lib)

target_include_directories(HyperEngine PUBLIC ${CMAKE_SOURCE_DIR}/vendor/SPIRV-Cross/)
target_link_libraries(HyperEngine PUBLIC spirv-cross-core)
target_link_libraries(HyperEngine PUBLIC spirv-cross-cpp)
target_link_libraries(HyperEngine PUBLIC spirv-cross-glsl)
target_link_libraries(HyperEngine PUBLIC spirv-cross-hlsl)
target_link_libraries(HyperEngine PUBLIC spirv-cross-reflect)

#-------------------------------------------------------------------------------------------
# Vulkan SDK
#-------------------------------------------------------------------------------------------
message(STATUS "Searching for Vulkan...")
find_package(Vulkan)

if(NOT Vulkan_FOUND)
    message(STATUS "Vulkan not found!")
    target_compile_definitions(HyperEngine PUBLIC GLFW_INCLUDE_NONE)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_VULKAN=0)
else()
    message(STATUS "Vulkan was found!")
    target_link_libraries(HyperEngine PUBLIC Vulkan::Vulkan)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_VULKAN=1)
endif()

hyperengine_set_output_directories(HyperEngine)
