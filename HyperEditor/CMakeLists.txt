#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#-------------------------------------------------------------------------------------------
# HyperEditor
#-------------------------------------------------------------------------------------------
file(GLOB_RECURSE "C_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "C_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE "CXX_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE "CXX_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

set(SOURCES "${C_SOURCES}" "${CXX_SOURCES}")
set(HEADERS "${C_HEADERS}" "${CXX_HEADERS}")

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")
add_executable(HyperEditor "${SOURCES}" "${HEADERS}")

target_include_directories(HyperEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/")

target_link_libraries(HyperEditor PRIVATE HyperEngine)
target_link_libraries(HyperEditor PRIVATE HyperResource)

target_link_libraries(HyperEditor PRIVATE imgui)
target_include_directories(HyperEditor PRIVATE "${CMAKE_SOURCE_DIR}/vendor/imgui/include/")

target_link_libraries(HyperEditor PRIVATE ImGuizmo)
target_include_directories(HyperEditor PRIVATE "${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/include/")

hyperengine_set_output_directories(HyperEditor)
hyperengine_post_copy_assets(HyperEditor)

set_property(TARGET HyperEditor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEditor/${CMAKE_BUILD_TYPE}/")
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT HyperEditor)