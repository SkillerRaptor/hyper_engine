#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#-------------------------------------------------------------------------------------------
# Project
#-------------------------------------------------------------------------------------------
set(PROJECT "Blank")

file(GLOB_RECURSE "C_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "C_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE "CXX_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE "CXX_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

set(SOURCES "${C_SOURCES}" "${CXX_SOURCES}")
set(HEADERS "${C_HEADERS}" "${CXX_HEADERS}")

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")
add_executable(${PROJECT} "${SOURCES}" "${HEADERS}")

target_include_directories(${PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries(${PROJECT} PRIVATE HyperEngine)

hyperengine_set_output_directories(${PROJECT})
hyperengine_post_copy_assets(${PROJECT})